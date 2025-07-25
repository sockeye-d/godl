#include "versioncommand.h"
#include "cli/ansi.h"
#include "cli/interface.h"
#include "versionregistry.h"

namespace cli::version {

int list(const Parser &parser)
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
        qStdOut() << asColumns(table) << note()
                  << "specify a repo and tag to narrow the search further with --repo and --tag"
                  << ansi::nl << note()
                  << "specify more filter terms (comma-separated) to narrow the search further";
        return 1;
    }
    const GodotVersion *version = versions.constFirst();
    version->start(showOutput);
    return 0;
}

} // namespace cli::version
