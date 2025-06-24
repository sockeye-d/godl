#ifndef VERSIONREGISTRY_H
#define VERSIONREGISTRY_H

#include <QMap>
#include <QString>
#include <QtQml/qqmlregistration.h>
#include "config.h"
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>

class VersionRegistry : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    KSharedConfig::Ptr config = KSharedConfig::openConfig(Config::godotLocation() / "godlversions");

public:
    VersionRegistry(QObject *parent = nullptr)
        : QObject(parent)
    {
        connect(Config::self(), &Config::godotLocationChanged, this, [this]() {
            config = KSharedConfig::openConfig(Config::godotLocation() / "godlversions");
        });
    }
    static VersionRegistry &instance()
    {
        static VersionRegistry registry = new VersionRegistry;
        return registry;
    }

    Q_INVOKABLE QString versionPath(QString versionTag) const;
    Q_INVOKABLE QString absoluteVersionPath(QString versionTag) const;
    Q_INVOKABLE void registerVersion(QString versionTag, QString executable);

    Q_INVOKABLE QMap<QString, QString> registeredVersions() const;
    Q_INVOKABLE QStringList registeredVersionTags() const;
};

#endif // VERSIONREGISTRY_H
