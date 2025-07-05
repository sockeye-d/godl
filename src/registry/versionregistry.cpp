#include "versionregistry.h"

#include <QDir>
#include <KSharedConfig>
#include <qtmetamacros.h>

void VersionRegistry::add(GodotVersion *version)
{
    connect(version, &GodotVersion::cmdChanged, this, [version, this]() {
        version->writeTo(config());
    });
    model()->append(version);
    Q_EMIT downloadedChanged();
    Q_EMIT hasVersionChanged();
}

void VersionRegistry::registerVersion(GodotVersion *version)
{
    version->writeTo(m_config);
    add(version);
}

void VersionRegistry::removeVersion(GodotVersion *version)
{
    debug() << "attempted to remove" << *version;
    model()->remove(version);
    m_config->deleteGroup(version->assetName());
    auto path = QFileInfo(version->absolutePath()).path();
    debug() << "removing" << path;
    QDir(path).removeRecursively();
    debug() << "done removing" << path;
    Q_EMIT downloadedChanged();
    Q_EMIT hasVersionChanged();
    version->deleteLater();
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

bool VersionRegistry::downloaded(QString tag) const
{
    const QStringList groups = m_config->groupList();
    for (const QString &groupName : groups) {
        if (m_config->group(groupName).readEntry("tag") == tag) {
            return true;
        }
    }
    return false;
}

bool VersionRegistry::hasVersion(const BoundGodotVersion *version) const
{
    const QStringList groups = m_config->groupList();
    for (const QString &groupName : groups) {
        auto g = m_config->group(groupName);
        if (g.readEntry("tag") == version->tagName()
            && g.readEntry("isMono", false) == version->isMono()) {
            return true;
        }
    }
    return false;
}
