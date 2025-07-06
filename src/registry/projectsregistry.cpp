#include "projectsregistry.h"
#include <QDir>
#include "godotproject.h"
#include "util.h"
#include <KConfigGroup>
#include <qcontainerfwd.h>
#include <qset.h>

GodotProject *ProjectsRegistry::load(const QString filepath)
{
    auto project = GodotProject::load(filepath);
    if (!project) {
        debug() << "Failed to load project at " << filepath;
        return nullptr;
    }
    project->setFavorite(config().group(project->path()).readEntry("favorite", false));
    project->m_registry = this;
    m_internalModel.append(project);
    m_model->resort();
    return project;
}

void ProjectsRegistry::scan(const QString directory)
{
    const auto files = QDir(directory).entryList(QDir::Files);
    for (const QString &file : files) {
        if (file == "project.godot") {
            import(directory / file);
        }
    }

    const auto dirs = QDir(directory).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &dir : dirs) {
        scan(directory / dir);
    }
}

void ProjectsRegistry::import(const QString filepath)
{
    if (config().hasGroup(QFileInfo(filepath).path() / GodotProject::projectFilename)) {
        debug() << "Already loaded project at" << filepath;
        return;
    }
    auto project = load(filepath);
    if (!project) {
        // debug() << "Failed to load project at" << filepath;
        return;
    }
    config().group(project->path());
    config().group(project->path()).writeEntry("favorite", false);
    config().sync();
}

void ProjectsRegistry::remove(GodotProject *project, bool moveToTrash)
{
    if (moveToTrash) {
        QFile::moveToTrash(QFileInfo(project->path()).path());
    }
    m_internalModel.remove(project);
    config().deleteGroup(project->path(), KConfig::Persistent);
    config().sync();
    project->deleteLater();
}

void ProjectsRegistry::setFavorite(const GodotProject *project, bool favorite)
{
    config().group(project->path()).writeEntry("favorite", favorite);
    config().sync();
}

bool ProjectsRegistry::favorite(const GodotProject *project)
{
    if (!config().hasGroup(project->path()))
        return false;
    if (!config().group(project->path()).hasKey("favorite"))
        return false;

    return config().group(project->path()).readEntry("favorite", false);
}

QStringList ProjectsRegistry::tags() const
{
    QStringList tags;
    for (int i = 0; i < m_model->rowCount(); i++) {
        const QStringList &projectTags = m_model->project(i)->tags();
        for (const QString &tag : projectTags) {
            if (!tags.contains(tag))
                tags << tag;
        }
    }
    tags.sort();
    return tags;
}

ProjectsRegistry::ProjectsRegistry(QObject *parent)
    : QObject(parent)
{
    const QStringList paths = config().groupList();
    for (const QString &path : paths) {
        auto proj = load(path);
        if (!proj) {
            m_loadErrors.append(path);
            config().deleteGroup(path);
        }
    }
    config().sync();
}

KConfig &ProjectsRegistry::config()
{
    static KConfig config("godlprojects", KConfig::SimpleConfig);
    return config;
}
