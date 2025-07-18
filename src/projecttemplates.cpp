#include "projecttemplates.h"
#include "config.h"
#include "fileutil.h"
#include "util.h"

#include <QFileInfoList>

namespace {
void copyRecursive(const QString &src, const QString &dest)
{
    static QDir d{};
    const QFileInfoList dirs = getEntries(src, QDir::Dirs);
    for (const auto &dir : dirs) {
        d.mkpath(dir.absoluteFilePath());
    }

    d.mkpath(dest);
    const QFileInfoList files = getEntries(src, QDir::Files);
    for (const auto &file : files) {
        QFile::copy(file.absoluteFilePath(), dest / file.fileName());
    }
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
