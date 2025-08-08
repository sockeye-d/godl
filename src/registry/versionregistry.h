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
#include "singleton.h"
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>
#include <qobject.h>
#include <qtmetamacros.h>

class VersionRegistry : public QObject, public Singleton<VersionRegistry>
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(VersionRegistryModel *model READ model CONSTANT FINAL)

    KSharedConfig::Ptr m_config;

    QMap<QString, GodotVersion *> m_versions;
    VersionRegistryModel *m_model = new VersionRegistryModel(this);

    void add(GodotVersion *version);

public:
    const QString location() const { return locationDirectory() / "godlversions"; }
    const QString locationDirectory() const { return Config::godotLocation(); }

private:
    void refreshConfigFile();

public:
    KSharedConfig::Ptr config() const { return m_config; }

    VersionRegistry(QObject *parent = nullptr);

    VersionRegistryModel *model() const { return m_model; }

    void registerVersion(GodotVersion *version);
    Q_INVOKABLE void removeVersion(GodotVersion *version);

    const QMap<QString, GodotVersion *> &versions() const;
    GodotVersion *version(QString uniqueName) const;
    QList<GodotVersion *> find(const QStringList &assetFilters,
                               const QString &repo = "",
                               const QString &tag = "");
    const QStringList assets() const;
    Q_INVOKABLE bool downloaded(const QString &tag, const QString &repo) const;
    Q_INVOKABLE bool downloadedAsset(const QString &tag,
                                     const QString &repo,
                                     const QString &assetName) const;
    Q_INVOKABLE bool hasVersion(const BoundGodotVersion *version) const;
    Q_INVOKABLE QString findUniqueName(const BoundGodotVersion *version) const;
    Q_INVOKABLE const GodotVersion *findVersion(const BoundGodotVersion *version) const;
    Q_SIGNAL void downloadedChanged();
    Q_SIGNAL void hasVersionChanged();

    Q_INVOKABLE QStringList detectLeakedVersions() const;
    Q_INVOKABLE void deleteLeakedVersions(QStringList versions) const;
    Q_INVOKABLE QString resolveSourceUrl(QString source) const;
};

#endif // VERSIONREGISTRY_H
