#ifndef PROJECTSREGISTRY_H
#define PROJECTSREGISTRY_H

#include <QString>
#include <QStringList>
#include <QtQml/qqmlregistration.h>
#include "model/internalprojectsregistrymodel.h"
#include "model/projectsregistrymodel.h"
#include <KConfig>
#include <qtmetamacros.h>

class ProjectsRegistry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ProjectsRegistryModel *model READ model CONSTANT FINAL)
private:
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged FINAL)
    bool m_scanning = false;

    void setScanning(bool scanning)
    {
        if (m_scanning == scanning)
            return;
        m_scanning = scanning;
        Q_EMIT scanningChanged();
    }

public:
    bool scanning() const { return m_scanning; }

    Q_SIGNAL void scanningChanged();

    // KConfig m_config = KConfig("godlprojects", KConfig::SimpleConfig);
    static KConfig &config();
    InternalProjectsRegistryModel m_internalModel{this};
    ProjectsRegistryModel *m_model = new ProjectsRegistryModel(&m_internalModel, this);

private:
    Q_PROPERTY(QStringList loadErrors READ loadErrors CONSTANT FINAL)
    QStringList m_loadErrors = {};

    QStringList loadErrors() const { return m_loadErrors; }

public:
    ProjectsRegistry(QObject *parent = nullptr);

    static ProjectsRegistry *instance()
    {
        static auto registry = new ProjectsRegistry;
        return registry;
    }

    ProjectsRegistryModel *model() { return m_model; }

    GodotProject *loadCli(const QString &filepath);
    /**
     * @brief load Loads a project from the given filepath. This adds it to the model, but doesn't add it to the configuration.
     * @param filepath
     * @return 
     */
    GodotProject *load(const QString filepath, bool invalidate = true);
    Q_INVOKABLE void scan(const QString directory);
    Q_INVOKABLE void import(const QString filepath, bool invalidate = true);
    Q_INVOKABLE void remove(GodotProject *project, bool moveToTrash);
    Q_INVOKABLE void setFavorite(const GodotProject *project, bool favorite);
    Q_INVOKABLE bool favorite(const GodotProject *project);
    Q_INVOKABLE QStringList tags() const;
};

#endif // PROJECTSREGISTRY_H
