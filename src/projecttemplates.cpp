#include "projecttemplates.h"
#include "config.h"
#include "fileutil.h"
#include "util.h"

#include <QFileInfoList>
#include <QRandomGenerator>

namespace {
void copyRecursive(const QString &src, const QString &dest)
{
    static QDir d{};
    const QFileInfoList dirs = getEntries(src, QDir::Dirs);
    for (const auto &dir : dirs) {
        d.mkpath(dir.absoluteFilePath());
        copyRecursive(dir.absoluteFilePath(), dest / dir.fileName());
    }

    d.mkpath(dest);
    const QFileInfoList files = getEntries(src, QDir::Files);
    for (const auto &file : files) {
        QFile::copy(file.absoluteFilePath(), dest / file.fileName());
    }
}
bool isFileText(const QString &path, QString &out)
{
    QFile f(path);
    if (!f.open(QFile::ReadOnly))
        return false;
    const QByteArray ba = f.readAll();
    QStringDecoder cv(QStringDecoder::System, QStringDecoder::Flag::Default);
    out = cv.decode(ba);
    if (!cv.hasError()) {
        return true;
    }
    out = "";
    return false;
}
void copyTemplateRecursive(const QString &src,
                           const QString &dest,
                           const QMap<QString, QVariant> &replacements,
                           const QString &root)
{
    static QDir d{};
    const QFileInfoList dirs = getEntries(src, QDir::Dirs);
    for (const auto &dir : dirs) {
        copyTemplateRecursive(dir.absoluteFilePath(), dest / dir.fileName(), replacements, root);
    }

    d.mkpath(dest);
    const QFileInfoList files = getEntries(src, QDir::Files | QDir::Hidden);
    QString content;
    for (const auto &file : files) {
        if (file.fileName() == "metatemplate.json") {
            continue;
        }
        if (isFileText(file.absoluteFilePath(), content)) {
            for (const auto &r : replacements.asKeyValueRange()) {
                content.replace("{" % r.first % "}", r.second.toString());
            }
            QFile f(dest / file.fileName());
            f.open(QFile::WriteOnly);
            f.write(content.toUtf8());
            f.close();
        } else {
            QFile::copy(file.absoluteFilePath(), dest / file.fileName());
        }
    }
}
void copyTemplateRecursive(const QString &src,
                           const QString &dest,
                           const QMap<QString, QVariant> &replacements)
{
    copyTemplateRecursive(src, dest, replacements, src);
}
} // namespace

ProjectTemplates::ProjectTemplates(QObject *parent)
    : QObject{parent}
{}

void ProjectTemplates::rescan()
{
    const QFileInfoList dirs = getEntries(Config::templateLocation(), QDir::Dirs);
    QList<Template> templates;
    templates.reserve(dirs.size());
    for (const auto &dir : dirs) {
        if (QFile::exists(dir.absoluteFilePath() / "metatemplate.json")) {
            templates.append(Template(dir.fileName(), dir.absoluteFilePath() / "metatemplate.json"));
        }
    }
    debug() << "Found templates" << templates;
    setTemplates(templates);
}

void ProjectTemplates::extractDefault()
{
    if (QDir(Config::templateLocation() / "default").exists()) {
        return;
    }
    debug() << "Extracting templates";
    copyRecursive(":/templates/default", Config::templateLocation() / "default");
}

void ProjectTemplates::createProject(const QString &name,
                                     const QString &dest,
                                     const QVariant &replacements)
{
    debug() << "Creating project from template" << name << "at" << dest << "with replacements"
            << replacements.toMap();
    copyTemplateRecursive(templ(name).directory(), dest, replacements.toMap());
}

bool ProjectTemplates::isProjectValid(const QString &path) const
{
    return !QDir(path).exists();
}

QString ProjectTemplates::generateRandomName() const
{
#define random(list) list[QRandomGenerator::global()->bounded(list.size())]
    static const QStringList segments = {"action",      "spinning",     "spiky",       "light",
                                         "dark",        "gloomy",       "funny",       "space",
                                         "realistic",   "stylized",     "stealth",     "survival",
                                         "adventure",   "metroidvania", "text",        "puzzle",
                                         "difficult",   "easy",         "programming", "plasma",
                                         "multiplayer", "gacha",        "indie",       "casual"};
    static const QStringList tails = {"shooter",
                                      "platformer",
                                      "shmup",
                                      "brawler",
                                      "rouguelike",
                                      "roguelite",
                                      "simulation",
                                      "crpg",
                                      "mmorpg",
                                      "jrpg",
                                      "mmo"};

    QStringList result{};
    int length = QRandomGenerator::global()->generateDouble()
                     * QRandomGenerator::global()->generateDouble()
                     * QRandomGenerator::global()->generateDouble()
                     * QRandomGenerator::global()->generateDouble()
                     * QRandomGenerator::global()->generateDouble() * 30
                 + 2;
    for (int i = 0; i < length; i++) {
        result.append(random(segments));
    }
    result.append(random(tails));
    return result.join("-");
#undef random
}
