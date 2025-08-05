#include "versioncommand.h"
#include "cli/ansi.h"
#include "cli/interface.h"
#include "versionregistry.h"

namespace cli::version {

int list(const Parser &)
{
    QList<QStringList> columns;
    columns.append({"repository", "tag", "asset", "path"});
    const VersionRegistry *vr = VersionRegistry::instance();
    for (const auto &versions = vr->versions(); const auto &version : versions) {
        columns.append(
            {version->repo(), version->tag(), version->assetName(), version->absolutePath()});
    }
    qStdOut() << asColumns(columns, " ");

    return 0;
}

int remove(const Parser &parser)
{
    const QString &repo = parser.op("repo").param("repo");
    const QString &tag = parser.op("tag").param("tag");
    const QStringList &assetFilters = parser.op("remove").param("filter-term").split(",");
    const bool force = parser.set("force");

    GodotVersion *version;
    if (getVersion(version, repo, tag, assetFilters)) {
        return 1;
    }
    bool result = true;
    if (!force)
        result = prompt("Are you sure you want to remove " % version->assetName() % "?",
                        {"y", "n"},
                        1)
                 == "y";
    if (result) {
        VersionRegistry::instance()->removeVersion(version);
        qStdOut() << positive() << "removed version " << version->assetName();
    }
    return 0;
}

int run(const Parser &parser)
{
    const QString &repo = parser.op("repo").param("repo");
    const QString &tag = parser.op("tag").param("tag");
    const QStringList &assetFilters = parser.op("run").param("filter-term").split(",");
    const bool &showOutput = parser.set("output");

    GodotVersion *version;
    if (getVersion(version, repo, tag, assetFilters)) {
        return 1;
    }
    version->start(showOutput);
    return 0;
}

int command(const Parser &parser)
{
    const QString &repo = parser.op("repo").param("repo");
    const QString &tag = parser.op("tag").param("tag");
    const QStringList &assetFilters = parser.op("command").param("filter-term").split(",");
    const QString &cmd = parser.op("command").param("cmd");
    GodotVersion *version;
    if (getVersion(version, repo, tag, assetFilters)) {
        return 1;
    }

    if (cmd.isEmpty()) {
        qStdOut() << version->cmd() << ansi::nl;
        return 0;
    }

    version->setCmd(cmd);
    version->writeTo(VersionRegistry::instance()->config());

    return 0;
}

} // namespace cli::version

bool getVersion(GodotVersion *&out,
                const QString &repo,
                const QString &tag,
                const QStringList &assetFilters)
{
    const auto &versions = VersionRegistry::instance()->find(assetFilters, repo, tag);
    if (versions.isEmpty()) {
        qStdOut() << cli::error() << "couldn't find version "
                  << (repo.isEmpty() ? "<no repo specified>" : repo) << " "
                  << (tag.isEmpty() ? "<no tag specified>" : tag) << " " << assetFilters.join(",")
                  << cli::ansi::nl;
        qStdOut()
            << cli::note()
            << "if you should have this version installed, install it with the install command"
            << cli::ansi::nl;
        return true;
    }
    if (versions.size() > 1) {
        qStdOut() << cli::error() << "found more than one version:" << cli::ansi::reset
                  << cli::ansi::nl;
        QList<QStringList> table;
        table.append({"repository", "tag", "asset"});
        for (const auto &version : versions) {
            table.append({version->repo(), version->tag(), version->assetName()});
        }
        qStdOut() << cli::asColumns(table) << cli::note()
                  << "specify a repo and tag to narrow the search further with --repo and --tag"
                  << cli::ansi::nl << cli::note()
                  << "specify more filter terms (comma-separated) to narrow the search further";
        return true;
    }
    out = versions.constFirst();
    return false;
}
