#ifndef VERSIONREGISTRY_H
#define VERSIONREGISTRY_H

#include <QMap>
#include <QProcess>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include "boundgodotversion.h"
#include "config.h"
#include "godotversion.h"
#include "model/versionregistrymodel.h"
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>
#include <qobject.h>
#include <qtmetamacros.h>

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

    void registerVersion(GodotVersion *version);
    Q_INVOKABLE void removeVersion(GodotVersion *version);

    QMap<QString, GodotVersion *> versions() const;
    GodotVersion *version(QString assetName) const;
    const QStringList assets() const;
    Q_INVOKABLE bool downloaded(QString tag) const;
    Q_INVOKABLE bool hasVersion(const BoundGodotVersion *version) const;
    Q_INVOKABLE QString findAssetName(const BoundGodotVersion *version) const;
    Q_INVOKABLE const GodotVersion *findVersion(const BoundGodotVersion *version) const;
    Q_SIGNAL void downloadedChanged();
    Q_SIGNAL void hasVersionChanged();
};

#endif // VERSIONREGISTRY_H
