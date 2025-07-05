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
    if (assetName == "") {
        return nullptr;
    }
    if (!m_config->hasGroup(assetName)) {
        return nullptr;
    }
    const KConfigGroup &group = m_config->group(assetName);
    const auto version = new GodotVersion();
    version->setAssetName(assetName);
    version->setTag(group.readEntry("tag"));
    version->setPath(group.readEntry("path"));
    version->setSourceUrl(group.readEntry("sourceUrl"));
    version->setRepo(group.readEntry("repo"));
    version->setIsMono(group.readEntry("isMono", version->isMono()));
    version->setCmd(group.readEntry("cmd"));
    return version;
}

const QStringList VersionRegistry::assets() const
{
    return m_config->groupList();
}

bool VersionRegistry::downloaded(const QString &tag, const QString &repo) const
{
    const QStringList groups = m_config->groupList();
    for (const QString &groupName : groups) {
        auto g = m_config->group(groupName);
        if (g.readEntry("tag") == tag && g.readEntry("repo") == repo) {
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
            && g.readEntry("isMono", false) == version->isMono()
            && g.readEntry("repo") == version->repo()) {
            return true;
        }
    }
    return false;
}

QString VersionRegistry::findAssetName(const BoundGodotVersion *version) const
{
    const QStringList groups = m_config->groupList();
    for (const QString &groupName : groups) {
        auto g = m_config->group(groupName);
        if (g.readEntry("tag") == version->tagName()
            && g.readEntry("isMono", false) == version->isMono()
            && g.readEntry("repo") == version->repo()) {
            return groupName;
        }
    }
    return "";
}

const GodotVersion *VersionRegistry::findVersion(const BoundGodotVersion *v) const
{
    return version(findAssetName(v));
}

QStringList VersionRegistry::detectLeakedVersions() const
{
    const QStringList downloadedVersions = QDir(Config::godotLocation())
                                               .entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    const auto groups = config()->groupList();
    QStringList executables;
    QStringList leakedExecutables;
    for (const QString &group : groups) {
        executables << QFileInfo(config()->group(group).readEntry("path")).path();
    }
    for (const QString &version : downloadedVersions) {
        if (!executables.contains(version)) {
            leakedExecutables << Config::godotLocation() / version;
        }
    }

    return leakedExecutables;
}

void VersionRegistry::deleteLeakedVersions(QStringList versions) const
{
    for (const QString &version : versions) {
        debug() << "Removing" << version;
        QDir(version).removeRecursively();
    }
}
