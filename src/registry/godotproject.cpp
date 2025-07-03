#include "godotproject.h"

#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSettings>
#include <QUrl>
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
    auto project = new GodotProject();
    QFileInfo file(path);
    if (file.fileName() == "godlproject") {
        project->deserialize(KConfig(path, KConfig::SimpleConfig).group(""), path);
        return project;
    }

    if (file.fileName() == "project.godot") {
        auto s = QSettings(path, QSettings::IniFormat);
        project->setName(s.value("application/config/name").toString());
        project->setDescription(s.value("application/config/description").toString());
        project->setTags(getArray(s, "application/config/tags"));
        project->setGodotVersion(new BoundGodotVersion());
        project->setLastEditedTime(file.lastModified());
        project->setPath(file.path() / GodotProject::projectFilename);
        project->setProjectPath(path);
        return project;
    }

    delete project;
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
    CFG_WRITE(tags);
    CFG_WRITE(name);
    CFG_WRITE(description);
    CFG_WRITE(projectPath);
}

void GodotProject::deserialize(KConfigGroup config, QString path)
{
    if (config.hasGroup("version"))
        godotVersion()->deserialize(config.group("version"));
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

    auto config = KConfig(QFileInfo(path).path() / GodotProject::projectFilename,
                          KConfig::SimpleConfig);
    project->serialize(config.group(""));
    config.sync();

    return project;
}

void GodotProject::setFavorite(bool favorite)
{
    if (m_favorite == favorite)
        return;
    m_favorite = favorite;
    debug() << name() << favorite;
    m_registry->setFavorite(this, favorite);
    Q_EMIT favoriteChanged();
}

bool GodotProject::favorite() const
{
    return m_favorite;
}
