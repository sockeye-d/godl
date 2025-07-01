#ifndef PROJECTSREGISTRY_H
#define PROJECTSREGISTRY_H

#include <QMap>
#include <QProcess>
#include <QQmlEngine>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include "model/projectsregistrymodel.h"
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>

class ProjectsRegistry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ProjectsRegistryModel *model READ model CONSTANT FINAL)

    KSharedConfigPtr m_config = KSharedConfig::openConfig("godlprojects",
                                                          KSharedConfig::SimpleConfig);
    ProjectsRegistryModel *m_model = new ProjectsRegistryModel(this);

public:
    ProjectsRegistry(QObject *parent = nullptr)
        : QObject(parent)
    {}

    static ProjectsRegistry *instance()
    {
        static auto registry = new ProjectsRegistry;
        return registry;
    }

    ProjectsRegistryModel *model() { return m_model; }

    Q_INVOKABLE void scan(const QString directory);
    Q_INVOKABLE void import(const QString filepath);
};

#endif // PROJECTSREGISTRY_H
