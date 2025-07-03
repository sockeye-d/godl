#include "versionregistry.h"

#include <KSharedConfig>

void VersionRegistry::add(GodotVersion *version)
{
    connect(version, &GodotVersion::cmdChanged, this, [version, this]() {
        version->writeTo(config());
    });
    model()->append(version);
}

void VersionRegistry::registerVersion(GodotVersion *version)
{
    version->writeTo(m_config);
    add(version);
}

QMap<QString, GodotVersion *> VersionRegistry::versions() const
{
    QMap<QString, GodotVersion *> map;
    for (const QString &assetName : assets()) {
        map[assetName] = version(assetName);
    }
    return map;
}

GodotVersion *VersionRegistry::version(QString assetName) const
{
    const KConfigGroup &group = m_config->group(assetName);
    const auto version = new GodotVersion();
    version->setAssetName(assetName);
    version->setTag(group.readEntry("tag"));
    version->setPath(group.readEntry("path"));
    version->setSourceUrl(group.readEntry("sourceUrl"));
    version->setIsMono(group.readEntry("isMono", version->isMono()));
    version->setCmd(group.readEntry("cmd"));
    return version;
}

const QStringList VersionRegistry::assets() const
{
    return m_config->groupList();
}
Q_INVOKABLE GodotVersion *VersionRegistry::qversion(QString versionTag) const
{
    return version(versionTag);
}
