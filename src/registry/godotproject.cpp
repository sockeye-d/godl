#include "godotproject.h"

#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSettings>
#include "util.h"
#include <KConfig>

// namespace {
// const QRegularExpression &featureSplitter()
// {
//     static QRegularExpression featureSplitter("[^(]*\\(([^)]*)\\)");
//     return featureSplitter;
// }
// } // namespace

GodotProject::GodotProject(QObject *parent)
    : QObject{parent}
{}

void GodotProject::serialize(KConfigGroup config)
{
    godotVersion()->serialize(config.group("version"));
    CFG_WRITE(favorite);
    CFG_WRITE(tags);
    CFG_WRITE(name);
    CFG_WRITE(description);
    CFG_WRITE(lastEditedTime);
}

void GodotProject::deserialize(KConfigGroup config)
{
    godotVersion()->deserialize(config.group("version"));
    setFavorite(CFG_READ(favorite));
    setTags(CFG_READ(tags));
    setName(CFG_READ(name));
    setDescription(CFG_READ(description));
    setLastEditedTime(CFG_READ(lastEditedTime));
}

std::unique_ptr<GodotProject> GodotProject::load(const QString &path)
{
    auto project = std::make_unique<GodotProject>();
    QFileInfo file(path);
    if (file.fileName() == "godlproject") {
        project->deserialize(KConfig(path, KConfig::SimpleConfig).group(""));
        return project;
    }

    if (file.fileName() == "project.godot") {
        auto s = QSettings(path, QSettings::IniFormat);
        project->setName(s.value("application/config/name").toString());
        project->setDescription(s.value("application/config/description").toString());
        // if (s.contains("application/config/features")) {
        //     QVariant v = s.value("application/config/features");
        //     QString out;
        //     if (!v.toStringList().empty()) {
        //         out = v.toStringList().join(",");
        //     } else {
        //         out = v.toString();
        //     }
        //     QList features = featureSplitter().match(out).captured(1).split(",");
        //     auto ver = new BoundGodotVersion();
        //     ver->setTagName(features[0] + "-stable");
        //     ver->setIsMono(features.contains("C#"));
        //     project->setGodotVersion(ver);
        // }
        project->setPath(file.path() / GodotProject::projectFilename);
        return project;
    }

    return nullptr;
}
