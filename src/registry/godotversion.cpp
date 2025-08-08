#include "godotversion.h"
#include "boundgodotversion.h"

#include <QDesktopServices>
#include <QFileInfo>
#include <QProcess>
#include <KConfigGroup>

GodotVersion::GodotVersion(QString configGroup,
                           QString tag,
                           QString assetName,
                           QString sourceUrl,
                           QString repo,
                           QString path,
                           bool isMono,
                           QObject *parent)
    : QObject{parent}
    , m_configGroupName{configGroup}
    , m_isMono{isMono}
    , m_assetName{assetName}
    , m_sourceUrl{sourceUrl}
    , m_path{path}
    , m_tag{tag}
    , m_repo{repo}
{
    connect(Config::self(), &Config::godotLocationChanged, this, &GodotVersion::absolutePathChanged);
}

void GodotVersion::writeTo(KSharedConfig::Ptr config) const
{
    auto group = config->group(m_configGroupName);
    group.writeEntry("tag", tag());
    group.writeEntry("path", path());
    group.writeEntry("assetName", assetName());
    group.writeEntry("sourceUrl", sourceUrl());
    group.writeEntry("repo", repo());
    group.writeEntry("isMono", isMono());
    group.writeEntry("cmd", cmd());
    group.sync();
}

bool operator==(const GodotVersion &left, const GodotVersion &right)
{
    return left.isMono() == right.isMono() && left.assetName() == right.assetName()
           && left.tag() == right.tag() && left.repo() == right.repo();
}

QDebug operator<<(QDebug dbg, const GodotVersion &godotVersion)
{
    dbg << "GodotVersion(" << godotVersion.toString() << ")";
    return dbg;
}

void GodotVersion::showExternally() const
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(absolutePath()).absolutePath()));
}

void GodotVersion::start(bool showOutput) const
{
    QProcess proc;
    if (!showOutput)
        proc.setStandardOutputFile(QProcess::nullDevice());
    proc.setProgram(absolutePath());
    proc.startDetached();
}

BoundGodotVersion *GodotVersion::boundVersion() const
{
    auto ver = new BoundGodotVersion();
    ver->setTagName(tag());
    ver->setIsMono(isMono());
    ver->setRepo(repo());
    return ver;
}

QString GodotVersion::toString() const
{
    if (isMono())
        return repo() / tag() % "-mono" % " (" % assetName() % ")";
    else
        return repo() / tag() % " (" % assetName() % ")";
}
