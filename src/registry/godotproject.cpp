#include "godotproject.h"

#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSettings>
#include <QUrl>
#include "boundgodotversion.h"
#include "projectsregistry.h"
#include "util.h"
#include <KConfig>
#include <KConfigGroup>

namespace {
const QRegularExpression &arraySplitter()
{
    static QRegularExpression featureSplitter("[^(]*\\(([^)]*)\\)");
    return featureSplitter;
}
QStringList getArray(const QSettings &settings, QString value)
{
    if (settings.contains(value)) {
        QVariant v = settings.value(value);
        QString out;
        if (!v.toStringList().empty()) {
            out = v.toStringList().join(",");
        } else {
            out = v.toString();
        }
        return arraySplitter().match(out).captured(1).split(",");
    }
    return {};
}
GodotProject *loadInternal(const QString &path)
{
    QFileInfo file(path);
    if (!file.exists()) {
        return nullptr;
    }

    if (file.fileName() == "godlproject") {
        auto project = new GodotProject();
        project->deserialize(KConfig(path, KConfig::SimpleConfig).group(""), path);
        return project;
    }

    if (file.fileName() == "project.godot") {
        auto project = new GodotProject();
        auto s = QSettings(path, QSettings::IniFormat);
        project->setName(s.value("application/config/name").toString());
        project->setDescription(s.value("application/config/description").toString());
        project->setTags(getArray(s, "application/config/tags"));
        project->setLastEditedTime(file.lastModified());
        project->setPath(file.path() / GodotProject::projectFilename);
        project->setProjectPath(path);
        return project;
    }
    return nullptr;
}
} // namespace

GodotProject::GodotProject(QObject *parent)
    : QObject{parent}
{}

bool GodotProject::showInFolder() const
{
    return QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path()).path()));
}

void GodotProject::serialize(KConfigGroup config)
{
    if (godotVersion()) {
        godotVersion()->serialize(config.group("version"));
    }
    CFG_WRITE(tags);
    CFG_WRITE(name);
    CFG_WRITE(description);
    CFG_WRITE(projectPath);
}

void GodotProject::deserialize(KConfigGroup config, QString path)
{
    if (config.hasGroup("version")) {
        if (!godotVersion()) {
            setGodotVersion(new BoundGodotVersion());
        }
        godotVersion()->deserialize(config.group("version"));
    }
    setFavorite(CFG_READ(favorite));
    setTags(CFG_READ(tags));
    setName(CFG_READ(name));
    setDescription(CFG_READ(description));
    setPath(path);
    setProjectPath(CFG_READ(projectPath));
    setLastEditedTime(QFileInfo(QFileInfo(path).path() / "project.godot").lastModified());
}

GodotProject *GodotProject::load(const QString &path)
{
    auto project = loadInternal(path);
    if (project == nullptr) {
        return nullptr;
    }

    project->setConfig(
        new KConfig(QFileInfo(path).path() / GodotProject::projectFilename, KConfig::SimpleConfig));

    return project;
}

void GodotProject::save()
{
    if (config()) {
        serialize(config()->group(""));
        config()->sync();
    }
}

void GodotProject::setFavorite(bool favorite)
{
    if (m_favorite == favorite)
        return;
    m_favorite = favorite;
    m_registry->setFavorite(this, favorite);
    Q_EMIT favoriteChanged();
}

bool GodotProject::favorite() const
{
    return m_favorite;
}
