#include "versionregistry.h"

#include <KSharedConfig>

void VersionRegistry::add(std::shared_ptr<GodotVersion> version)
{
    connect(version.get(),
            &GodotVersion::cmdChanged,
            this,
            [weak_version = (std::weak_ptr<GodotVersion>) version, this]() {
                auto ptr = weak_version.lock();
                if (ptr)
                    ptr->writeTo(config());
            });
    model()->append(version);
}

void VersionRegistry::registerVersion(std::shared_ptr<GodotVersion> version)
{
    version->writeTo(m_config);
    add(version);
}

QMap<QString, std::shared_ptr<GodotVersion>> VersionRegistry::versions() const
{
    QMap<QString, std::shared_ptr<GodotVersion>> map;
    for (const QString &assetName : assets()) {
        map[assetName] = version(assetName);
    }
    return map;
}

std::shared_ptr<GodotVersion> VersionRegistry::version(QString assetName) const
{
    const KConfigGroup &group = m_config->group(assetName);
    const auto version = std::make_shared<GodotVersion>();
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
    return version(versionTag).get();
}
