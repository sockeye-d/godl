#include "versionregistry.h"

#include <KSharedConfig>

using namespace Qt::StringLiterals;

void VersionRegistry::registerVersion(QString versionTag, QString executable, bool isMono)
{
    config->group("").writeEntry(versionTag, executable);
    config->group("").writeEntry(versionTag, isMono);
    config->sync();
}

QMap<QString, QString> VersionRegistry::registeredVersions() const
{
    return config->entryMap();
}

QStringList VersionRegistry::registeredVersionTags() const
{
    return config->entryMap().keys();
}

QString VersionRegistry::versionPath(QString versionTag) const
{
    return config->group("").readEntry(versionTag, "");
}

QString VersionRegistry::absoluteVersionPath(QString versionTag) const
{
    return config->name() / versionPath(versionTag);
}
