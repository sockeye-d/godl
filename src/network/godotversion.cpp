#include "godotversion.h"

#include <QDesktopServices>
#include <QFileInfo>
#include <KConfigGroup>

GodotVersion::GodotVersion(
    QString tag, QString assetName, QString sourceUrl, QString path, bool isMono, QObject *parent)
    : QObject{parent}
    , m_isMono{isMono}
    , m_assetName{assetName}
    , m_sourceUrl{sourceUrl}
    , m_path{path}
    , m_tag{tag}
{
    connect(Config::self(), &Config::godotLocationChanged, this, &GodotVersion::absolutePathChanged);
}

void GodotVersion::writeTo(KSharedConfig::Ptr config) const
{
    auto group = config->group(assetName());
    group.writeEntry("tag", tag());
    group.writeEntry("path", path());
    group.writeEntry("assetName", assetName());
    group.writeEntry("sourceUrl", sourceUrl());
    group.writeEntry("isMono", isMono());
    group.writeEntry("cmd", cmd());
    group.sync();
}

bool operator==(const GodotVersion &left, const GodotVersion &right)
{
    return left.isMono() == right.isMono() && left.assetName() == right.assetName();
}

QDebug operator<<(QDebug dbg, const GodotVersion &godotVersion)
{
    dbg << "GodotVersion(" << godotVersion.assetName() << ")";
    return dbg;
}
void GodotVersion::showExternally() const
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(absolutePath()).absolutePath()));
}
