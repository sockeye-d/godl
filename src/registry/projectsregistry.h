#ifndef PROJECTSREGISTRY_H
#define PROJECTSREGISTRY_H

#include <QMap>
#include <QProcess>
#include <QQmlEngine>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include "model/internalprojectsregistrymodel.h"
#include "model/projectsregistrymodel.h"
#include "util.h"
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>
#include <ksharedconfig.h>

class ProjectsRegistry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ProjectsRegistryModel *model READ model CONSTANT FINAL)

    // KConfig m_config = KConfig("godlprojects", KConfig::SimpleConfig);
    KConfig &config()
    {
        static KConfig config("godlprojects", KConfig::SimpleConfig);
        return config;
    }
    InternalProjectsRegistryModel m_internalModel{this};
    ProjectsRegistryModel *m_model = new ProjectsRegistryModel(&m_internalModel, this);

public:
    ProjectsRegistry(QObject *parent = nullptr)
        : QObject(parent)
    {
        const QStringList paths = config().groupList();
        debug() << paths;
        for (const QString &path : paths) {
            load(path);
        }
    }
    // ~ProjectsRegistry() { m_config.reset(); }

    static ProjectsRegistry *instance()
    {
        static auto registry = new ProjectsRegistry;
        return registry;
    }

    ProjectsRegistryModel *model() { return m_model; }

    /**
     * @brief load Loads a project from the given filepath. This adds it to the model, but doesn't add it to the configuration.
     * @param filepath
     * @return 
     */
    GodotProject *load(const QString filepath);
    Q_INVOKABLE void scan(const QString directory);
    Q_INVOKABLE void import(const QString filepath);
    Q_INVOKABLE void remove(GodotProject *project);
    Q_INVOKABLE void setFavorite(const GodotProject *project, bool favorite);
    Q_INVOKABLE bool favorite(const GodotProject *project);
};

#endif // PROJECTSREGISTRY_H
