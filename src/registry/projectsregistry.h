#ifndef PROJECTSREGISTRY_H
#define PROJECTSREGISTRY_H

#include <QMap>
#include <QProcess>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include "model/projectsregistrymodel.h"
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>

class ProjectsRegistry : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    Q_PROPERTY(ProjectsRegistryModel *model READ model CONSTANT FINAL)

    KSharedConfigPtr m_config = KSharedConfig::openConfig("godlprojects",
                                                          KSharedConfig::SimpleConfig);
    ProjectsRegistryModel *m_model = new ProjectsRegistryModel(this);

public:
    ProjectsRegistry(QObject *parent = nullptr)
        : QObject(parent)
    {}

    ProjectsRegistryModel *model() { return m_model; }

    Q_INVOKABLE void scan(QString directory);
};

#endif // PROJECTSREGISTRY_H
