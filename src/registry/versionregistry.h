#ifndef VERSIONREGISTRY_H
#define VERSIONREGISTRY_H

#include <QMap>
#include <QProcess>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include "config.h"
#include "model/versionregistrymodel.h"
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>

class VersionRegistry : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(VersionRegistryModel *model READ model CONSTANT FINAL)

    KSharedConfig::Ptr m_config;

    VersionRegistryModel *m_model = new VersionRegistryModel(this);

    void add(GodotVersion *version);

    void refreshConfigFile()
    {
        m_config = KSharedConfig::openConfig(Config::godotLocation() / "godlversions",
                                             KSharedConfig::SimpleConfig);
        model()->clear();
        for (auto &version : versions()) {
            add(version);
        }
    }

    KSharedConfig::Ptr config() const { return m_config; }

public:
    VersionRegistry(QObject *parent = nullptr)
        : QObject(parent)
    {
        refreshConfigFile();
        connect(Config::self(),
                &Config::godotLocationChanged,
                this,
                &VersionRegistry::refreshConfigFile);
    }
    static VersionRegistry *instance()
    {
        static auto registry = new VersionRegistry;
        return registry;
    }

    VersionRegistryModel *model() const { return m_model; }

    Q_INVOKABLE GodotVersion *qversion(QString versionTag) const;
    void registerVersion(GodotVersion *version);

    QMap<QString, GodotVersion *> versions() const;
    GodotVersion *version(QString assetName) const;
    const QStringList assets() const;
};

#endif // VERSIONREGISTRY_H
