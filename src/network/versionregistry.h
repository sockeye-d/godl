#ifndef VERSIONREGISTRY_H
#define VERSIONREGISTRY_H

#include <QMap>
#include <QString>
#include <KConfig>
#include <KConfigGroup>

namespace VersionRegistry {

QString versionPath(QString versionTag);

void registerVersion(QString versionTag, QString executable);
} // namespace VersionRegistry

#endif // VERSIONREGISTRY_H
