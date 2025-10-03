#include "versionregistry.h"
#include "cli/ansi.h"
#include "fileutil.h"

#include <KSharedConfig>
#include <qtmetamacros.h>
#include <QDir>
#include <QRegularExpression>

void VersionRegistry::add(GodotVersion *version)
{
    connect(version, &GodotVersion::cmdChanged, this, [version, this]() {
        version->writeTo(config());
    });
    m_versions[version->m_configGroupName] = version;
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
    print_debug() << "attempted to remove" << *version;
    model()->remove(version);
    m_config->deleteGroup(version->m_configGroupName);
    m_config->sync();
    if (version->absolutePath().startsWith(Config::godotLocation())) {
        auto path = Config::godotLocation() / getPathRoot(version->path());
        print_debug() << "removing" << path;
        QDir(path).removeRecursively();
        print_debug() << "done removing" << path;
    } else {
        print_debug() << "not deleting";
    }
    Q_EMIT downloadedChanged();
    Q_EMIT hasVersionChanged();
    m_versions.remove(version->m_configGroupName);
    version->deleteLater();
}

const QMap<QString, GodotVersion *> &VersionRegistry::versions() const
{
    return m_versions;
}

GodotVersion *VersionRegistry::version(QString uniqueName) const
{
    if (versions().contains(uniqueName)) {
        return versions().value(uniqueName);
    }
    if (uniqueName == "") {
        return nullptr;
    }
    if (!m_config->hasGroup(uniqueName)) {
        return nullptr;
    }
    const KConfigGroup &group = m_config->group(uniqueName);
    const auto version = new GodotVersion();
    version->m_configGroupName = uniqueName;
    version->setAssetName(group.readEntry("assetName"));
    version->setTag(group.readEntry("tag"));
    version->setPath(group.readEntry("path"));
    version->setSourceUrl(group.readEntry("sourceUrl"));
    version->setRepo(group.readEntry("repo"));
    version->setIsMono(group.readEntry("isMono", version->isMono()));
    version->setCmd(group.readEntry("cmd"));
    return version;
}

QList<GodotVersion *> VersionRegistry::find(const QStringList &assetFilters,
                                            const QString &repo,
                                            const QString &tag)
{
    QList<GodotVersion *> versions;
    for (const auto &allVersions = this->versions(); const auto &version : allVersions) {
        if ((repo.isEmpty() || version->repo() == repo)
            && (tag.isEmpty() || version->tag() == tag)) {
            for (const QString &filter : assetFilters) {
                if (!version->assetName().contains(filter)) {
                    goto notFound;
                }
            }
            versions.append(version);
        }
    notFound:;
    }
    return versions;
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

bool VersionRegistry::downloadedAsset(const QString &tag,
                                      const QString &repo,
                                      const QString &assetName) const
{
    const QStringList groups = m_config->groupList();
    for (const QString &groupName : groups) {
        auto g = m_config->group(groupName);
        if (g.readEntry("tag") == tag && g.readEntry("repo") == repo
            && g.readEntry("assetName") == assetName) {
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

QString VersionRegistry::findUniqueName(const BoundGodotVersion *version) const
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
    return version(findUniqueName(v));
}

bool VersionRegistry::canAutodetect() const
{
    static bool hasGit = !QStandardPaths::findExecutable("git").isEmpty();
    return hasGit;
}

QString execute(const QString &executable, const QString &cwd, const QStringList &args)
{
    QProcess proc;
    proc.setWorkingDirectory(cwd);
    proc.start(executable, args);
    proc.waitForFinished(-1);
    return proc.readAllStandardOutput();
}

bool VersionRegistry::canAutodetectFile(const QString &path) const
{
    if (path.isEmpty()) {
        return false;
    }

    const QString git = QStandardPaths::findExecutable("git");
    if (git.isEmpty()) {
        print_debug() << "Git executable not found";
        return false;
    }

    if (execute(git, QFileInfo(path).path(), {"rev-parse", "--is-inside-work-tree"}).trimmed()
        == "true") {
        return true;
    }
    return false;
}

QString VersionRegistry::detectRepository(const QString &path) const
{
    if (path.isEmpty()) {
        return "";
    }

    const static QRegularExpression lineSplitter = QRegularExpression("[\r\n]");
    const QString git = QStandardPaths::findExecutable("git");
    if (git.isEmpty()) {
        print_debug() << "Git executable not found";
        return "";
    }

    const QString dir = QFileInfo(path).path();
    const QString remoteName
        = execute(git, dir, {"remote", "show"}).split(lineSplitter, Qt::SkipEmptyParts).constFirst();
    QString pushUrl = removePrefix(execute(git,
                                           dir,
                                           {"config", "--local", "remote." + remoteName + ".url"})
                                       .trimmed(),
                                   "https://");

    if (!pushUrl.startsWith("github.com")) {
        return "";
    }

    pushUrl = removePrefix(pushUrl, "github.com");
    pushUrl = removeSuffix(pushUrl, ".git");

    return pushUrl;
}

QString VersionRegistry::detectTag(const QString &path) const
{
    if (path.isEmpty()) {
        return "";
    }

    const QString git = QStandardPaths::findExecutable("git");
    if (git.isEmpty()) {
        print_debug() << "Git executable not found";
        return "";
    }
    const QString dir = QFileInfo(path).path();

    return execute(git, dir, {"describe", "--tags", "--abbrev=1"}).trimmed();
}

QString VersionRegistry::detectAsset(const QString &path) const
{
    if (path.isEmpty()) {
        return "";
    }

    return removeSuffix(QFileInfo(path).fileName(), "exe");
}

bool VersionRegistry::detectMono(const QString &path) const
{
    if (path.isEmpty()) {
        return "";
    }

    return QFileInfo(path).fileName().contains("mono");
}

QStringList VersionRegistry::detectLeakedVersions() const
{
    const QStringList downloadedVersions = QDir(Config::godotLocation())
                                               .entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    const auto groups = config()->groupList();
    QStringList executables;
    QStringList leakedExecutables;
    for (const QString &group : groups) {
        executables << getPathRoot(config()->group(group).readEntry("path"));
    }
    for (const QString &version : downloadedVersions) {
        if (executables.contains(version)) {
            continue;
        }

        leakedExecutables << Config::godotLocation() / version;
    }

    return leakedExecutables;
}

void VersionRegistry::deleteLeakedVersions(QStringList versions) const
{
    for (const QString &version : versions) {
        print_debug() << "Removing" << version;
        QDir(version).removeRecursively();
    }
}

QString VersionRegistry::resolveSourceUrl(QString source) const
{
    if (source.startsWith("/"))
        return "https://api.github.com/repos" + source;
    else
        return source;
}

void VersionRegistry::registerLocalVersion(
    const QString &path, const QString &repo, const QString &tag, const QString &asset, bool isMono)
{
    const QString uniqueName = getUniqueName(asset);
    const auto version = new GodotVersion();
    version->m_configGroupName = uniqueName;
    version->setAssetName(asset);
    version->setTag(tag);
    version->setPath(path);
    version->setSourceUrl("");
    version->setRepo(repo);
    version->setIsMono(isMono);
    version->setCmd(Config::defaultCommand());
    registerVersion(version);
}

void VersionRegistry::refreshConfigFile()
{
    m_config = KSharedConfig::openConfig(location(), KSharedConfig::SimpleConfig);
    for (GodotVersion *version : std::as_const(m_versions)) {
        version->deleteLater();
    }
    m_versions.clear();
    model()->clear();
    for (const auto &groupName : m_config->groupList()) {
        add(version(groupName));
    }
}

VersionRegistry::VersionRegistry(QObject *parent)
    : QObject(parent)
{
    refreshConfigFile();
    connect(Config::self(),
            &Config::godotLocationChanged,
            this,
            &VersionRegistry::refreshConfigFile);
}

QString VersionRegistry::sanitizeAssetName(const QString &assetName, const QString &withWhat) const
{
    static QRegularExpression re{"[<!>:\"\\/\\|?*]", QRegularExpression::MultilineOption};
    return QString(assetName).replace(re, withWhat);
}

QString VersionRegistry::getUniqueName(const QString &assetName) const
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces) % "-" % sanitizeAssetName(assetName);
}
