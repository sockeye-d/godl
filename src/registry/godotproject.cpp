#include "godotproject.h"
#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSettings>
#include <QUrl>
#include "boundgodotversion.h"
#include "projectsregistry.h"
#include "serializable.h"
#include "util.h"
#include "versionregistry.h"
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
        project->setIcon(s.value("application/config/icon").toString().replace("res://", ""));
        project->setLastEditedTime(file.lastModified());
        project->setPath(file.path() / GodotProject::projectFilename);
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
    } else if (config.hasGroup("version")) {
        config.deleteGroup("version");
    }
    CFG_WRITE(tags);
    CFG_WRITE(name);
    CFG_WRITE(description);
    CFG_WRITE(icon);
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
    setIcon(CFG_READ(icon));
    setPath(path);
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

GodotProject::OpenError GodotProject::open() const
{
    if (!godotVersion()) {
        return GodotProject::NoEditorBound;
    }

    auto v = VersionRegistry::instance()->findVersion(godotVersion());

    if (!v) {
        return GodotProject::NoEditorFound;
    }

    QString cmd = v->cmd()
                      .replace("{executable}", v->absolutePath())
                      .replace("{projectPath}", projectPath());
    debug() << "Starting" << cmd;
    QStringList args = QProcess::splitCommand(cmd);
    QString exe = args.first();
    args.removeFirst();
    if (!QProcess::startDetached(exe, args)) {
        return GodotProject::FailedToStartEditor;
    }

    return GodotProject::NoError;
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

QString GodotProject::projectPath() const
{
    return QFileInfo(path()).path() / "project.godot";
}

QUrl GodotProject::iconSource() const
{
    return QUrl::fromLocalFile(QFileInfo(path()).path() / icon());
}
