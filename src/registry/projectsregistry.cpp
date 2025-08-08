#include "projectsregistry.h"
#include <QDir>
#include <QFutureWatcher>
#include <QSet>
#include <QtConcurrentRun>
#include "godotproject.h"
#include "util.h"
#include <KConfigGroup>

namespace {
QStringList scanInternal(const QString &directory)
{
    QStringList result{};
    const auto files = QDir(directory).entryList(QDir::Files);
    for (const QString &file : files) {
        if (file == "project.godot") {
            return {directory / file};
        }
    }

    const auto dirs = QDir(directory).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &dir : dirs) {
        result.append(scanInternal(directory / dir));
    }
    return result;
}
} // namespace

GodotProject *ProjectsRegistry::load(const QString &filepath, bool invalidate)
{
    auto project = GodotProject::load(filepath);
    if (!project) {
        print_debug() << "Failed to load project at " << filepath;
        return nullptr;
    }
    project->setFavorite(config().group(project->path()).readEntry("favorite", false));
    project->m_registry = this;
    m_internalModel.append(project);
    if (invalidate)
        m_model->invalidate();
    return project;
}

void ProjectsRegistry::scan(const QString &directory, QStringList *out, bool dryRun)
{
    setScanning(true);

    QFuture<QStringList> future = QtConcurrent::run([directory](QPromise<QStringList> &promise) {
        promise.start();
        promise.addResult(scanInternal(directory));
        promise.finish();
    });

    auto watcher = new QFutureWatcher<QStringList>();
    connect(watcher,
            &QFutureWatcher<QString>::finished,
            this,
            [this, watcher, future, out, dryRun]() {
                const QStringList &results = future.result();
                if (out) {
                    *out = results;
                }
                if (!dryRun) {
                    for (const QString &result : results) {
                        import(result, result == results.constLast());
                    }
                }
                watcher->deleteLater();
                setScanning(false);
            });
    watcher->setFuture(future);
}

ProjectsRegistry::ImportError ProjectsRegistry::import(const QString &filepath, bool invalidate)
{
    if (config().hasGroup(QFileInfo(filepath).path() / GodotProject::projectFilename)) {
        print_debug() << "Already loaded project at" << filepath;
        return AlreadyLoaded;
    }
    auto project = load(filepath, invalidate);
    if (!project) {
        project = load(filepath / "godlproject", invalidate);
    }
    if (!project) {
        project = load(filepath / "project.godot", invalidate);
    }
    if (!project) {
        return CantLoad;
    }
    config().group(project->path()).writeEntry("favorite", false);
    if (invalidate) {
        config().sync();
    }

    return LoadedSucessfully;
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
        auto proj = load(path, false);
        if (!proj) {
            m_loadErrors.append(path);
            print_debug() << "Couldn't find project at" << path;
            config().deleteGroup(path);
        }
    }
    config().sync();

    m_model->invalidate();
    m_model->resort();
}

GodotProject *ProjectsRegistry::loadCli(const QString &filepath)
{
    auto project = GodotProject::load(filepath);
    if (!project) {
        return nullptr;
    }
    project->setFavorite(config().group(project->path()).readEntry("favorite", false));
    return project;
}

KConfig &ProjectsRegistry::config()
{
    static KConfig config(QStandardPaths::standardLocations(QStandardPaths::AppDataLocation)
                                  .constFirst()
                              / "godlprojects",
                          KConfig::SimpleConfig);
    return config;
}
