#ifndef VERSIONREGISTRY_H
#define VERSIONREGISTRY_H

#include <QMap>
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

    void add(std::shared_ptr<GodotVersion> version);

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
    static VersionRegistry &instance()
    {
        static VersionRegistry registry = new VersionRegistry;
        return registry;
    }

    VersionRegistryModel *model() const { return m_model; }

    Q_INVOKABLE GodotVersion *qversion(QString versionTag) const;
    void registerVersion(std::shared_ptr<GodotVersion> version);

    QMap<QString, std::shared_ptr<GodotVersion>> versions() const;
    std::shared_ptr<GodotVersion> version(QString assetName) const;
    const QStringList assets() const;
};

#endif // VERSIONREGISTRY_H
