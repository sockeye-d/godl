#include "versioncommand.h"
#include "cli/ansi.h"
#include "cli/interface.h"
#include "versionregistry.h"

namespace cli::version {

int list(const Parser &)
{
    QList<QStringList> columns;
    columns.append({"repository", "tag", "asset"});
    const VersionRegistry *vr = VersionRegistry::instance();
    for (const auto &versions = vr->versions(); const auto &version : versions) {
        columns.append({version->repo(), version->tag(), version->assetName()});
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

    VersionRegistry *vr = VersionRegistry::instance();
    const auto &versions = vr->find(assetFilters, repo, tag);
    if (versions.isEmpty()) {
        qStdOut() << error() << "couldn't find version " << repo << " " << tag << " "
                  << assetFilters.join(",") << ansi::nl;
        return 1;
    }
    if (versions.size() > 1) {
        qStdOut() << error() << "found more than one version:" << ansi::reset << ansi::nl;
        QList<QStringList> table;
        table.append({"repository", "tag", "asset"});
        for (const auto &version : versions) {
            table.append({version->repo(), version->tag(), version->assetName()});
        }
        qStdOut() << asColumns(table);
        return 1;
    }
    const GodotVersion *version = versions.constFirst();
    bool result = true;
    if (!force)
        result = prompt("Are you sure you want to remove " % version->assetName() % "?",
                        {"y", "n"},
                        1)
                 == "y";
    if (result) {
        vr->removeVersion(versions.constFirst());
        qStdOut() << positive() << "removed version " << version->assetName();
    }
    return 0;
}

int run(const Parser &parser)
{
    const QString &repo = parser.op("repo").param("repo");
    const QString &tag = parser.op("tag").param("tag");
    const bool &showOutput = parser.set("output");
    const QStringList &assetFilters = parser.op("run").param("filter-term").split(",");

    GodotVersion *version;
    if (getVersion(version, repo, tag, assetFilters)) {
        return 1;
    }
    version->start(showOutput);
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
        qStdOut() << cli::error() << "couldn't find version " << repo << " " << tag << " "
                  << assetFilters.join(",") << cli::ansi::nl;
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
