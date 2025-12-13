#include "remotecommand.h"
#include "chainedjsonrequest.h"
#include "cli/ansi.h"
#include "cli/interface.h"
#include "util.h"

#include <QCoreApplication>
#include <QRegularExpression>
#include <QThread>
#include "versionregistry.h"

using namespace Qt::StringLiterals;
using namespace std::chrono_literals;

namespace cli::remote {

int list(const Parser &parser)
{
    const QString repo = parser.op("repo").param("repo",
                                                 firstOrDefault(Config::sources(),
                                                                u"/godotengine/godot-builds"_s));
    const QString tag = parser.op("list").param("tag");
    bool listAll = parser.set("list-all");
    auto req = new ChainedJsonRequest;
    auto del = qScopeGuard([req]() { delete req; });
    bool finished = false;
    QVariant fetchResult = QVariant();
    QString errorString = "";
    const auto &finishedConnection
        = req->connect(req,
                       &ChainedJsonRequest::finished,
                       req,
                       [&finished,                              // clazy:exclude=lambda-in-connect
                        &fetchResult](const QVariant &result) { // clazy:exclude=lambda-in-connect
                           fetchResult = result;
                           finished = true;
                       });
    const auto &errorConnection
        = req->connect(req,
                       &ChainedJsonRequest::error,
                       req,
                       [&finished,            // clazy:exclude=lambda-in-connect
                        &errorString](size_t, // clazy:exclude=lambda-in-connect
                                      const QNetworkReply::NetworkError &,
                                      const QString &es) {
                           finished = true;
                           errorString = es;
                       });
    req->execute(
        {u"%1/releases/tags/%2"_s.arg(VersionRegistry::instance()->resolveSourceUrl(repo), tag)});

    std::chrono::nanoseconds nowTime = std::chrono::system_clock::now().time_since_epoch();
    qStdOut() << ansi::cursorHide;
    qStdOut().flush();
    while (!finished) {
        QCoreApplication::processEvents();
        qStdOut() << ansi::cr << ansi::eraseInLine
                  << progressBar(nowTime, "Fetching assets for " % repo % "/" % tag);
        qStdOut().flush();
        QThread::sleep(10ms);
    }
    req->disconnect(finishedConnection);
    req->disconnect(errorConnection);
    if (!errorString.isEmpty()) {
        qStdOut() << ansi::nl << error() << "fetch failed with error " << errorString;
        return 1;
    }
    QList<QStringList> columns;
    columns.append({" ", "repository", "tag", "asset"});
    for (const auto &versions = fetchResult.toList().constFirst().toMap().value("assets").toList();
         const auto &version : versions) {
        const QString &assetName = version.toMap().value("name").toString();
        bool recommended = false;
        for (const QStringList &filters = Config::downloadFilter();
             const QString &filter : filters) {
            if (assetName.contains(filter)) {
                recommended = true;
                break;
            }
        }
        if (!listAll && !recommended) {
            continue;
        }
        columns.append({recommended ? ansi::fgcolor(ansi::Green) + "✔" : " ",
                        repo,
                        tag,
                        assetName + ansi::reset});
    }
    qStdOut() << ansi::nl << asColumns(columns, " ");
    if (!listAll)
        qStdOut() << note() << "list all versions with the --all flag" << ansi::nl;
    return 0;
}

int listTags(const Parser &parser)
{
    const static QRegularExpression nextPageRegex{"page=(\\d+)>; rel=\"last\""};
    const QString repo = parser.op("repo").param("repo",
                                                 firstOrDefault(Config::sources(),
                                                                u"/godotengine/godot-builds"_s));
    bool listAll = parser.set("list-all");
    auto req = new ChainedJsonRequest;
    auto del = qScopeGuard([req]() { delete req; });
    bool finished = false;
    QVariant fetchResult = QVariant();
    QString errorString = "";
    QString baseUrl = u"%1/tags?per_page=%2"_s
                          .arg(VersionRegistry::instance()->resolveSourceUrl(repo))
                          .arg(listAll ? 100 : 25);
    int lastPage = listAll ? -1 : 1;
    int currentPage = 0;
    QVariantList tagsVariant;
    print_debug() << baseUrl;
    auto xform =
        [&lastPage, &currentPage, &tagsVariant, baseUrl](const QVariant &result,
                                                         const QVariant &headers) -> QVariant {
        if (lastPage == -1) {
            const auto &match = nextPageRegex.match(headers.toMap().value("link").toString());
            if (match.hasMatch()) {
                lastPage = match.capturedTexts().at(1).toInt();
            }
        }
        tagsVariant.append(result.toList());
        currentPage++;
        if (currentPage == lastPage) {
            return QVariantList();
        }
        QString url = u"%1&page=%2"_s.arg(baseUrl).arg(currentPage + 1);
        return QVariantList({url});
    };
    // cursed... but I do this in QML and it works there 💀
    for (int i = 0; i < 100; i++) {
        req->addStep(xform);
    }
    const auto &finishedConnection
        = req->connect(req,
                       &ChainedJsonRequest::finished,
                       req,
                       [&finished,                              // clazy:exclude=lambda-in-connect
                        &fetchResult](const QVariant &result) { // clazy:exclude=lambda-in-connect
                           fetchResult = result;
                           finished = true;
                       });
    const auto &errorConnection
        = req->connect(req,
                       &ChainedJsonRequest::error,
                       req,
                       [&finished,            // clazy:exclude=lambda-in-connect
                        &errorString](size_t, // clazy:exclude=lambda-in-connect
                                      const QNetworkReply::NetworkError &,
                                      const QString &es) {
                           finished = true;
                           errorString = es;
                       });
    req->execute({baseUrl});

    std::chrono::nanoseconds nowTime = std::chrono::system_clock::now().time_since_epoch();
    qStdOut() << ansi::cursorHide;
    qStdOut().flush();
    while (!finished) {
        QCoreApplication::processEvents();
        qStdOut() << ansi::cr << ansi::eraseInLine
                  << progressBar(nowTime,
                                 u"Fetching tags for %1, page %2"_s.arg(repo).arg(currentPage));
        qStdOut().flush();
        QThread::sleep(10ms);
    }
    req->disconnect(finishedConnection);
    req->disconnect(errorConnection);
    if (!errorString.isEmpty()) {
        qStdOut() << ansi::nl << error() << "fetch failed with " << errorString;
        return 1;
    }
    QList<QStringList> tags;
    std::reverse(tagsVariant.begin(), tagsVariant.end());
    QStringList row;
    row.reserve(5);
    for (const auto &tag : tagsVariant) {
        row.append(tag.toMap().value("name").toString());
        if (row.size() >= 5) {
            tags.append(row);
            row.clear();
        }
    }
    tags.append(row);
    qStdOut() << ansi::nl << asColumns(tags, " ", true);
    if (!listAll)
        qStdOut() << note() << "list all tags with the --all flag" << ansi::nl;
    qStdOut() << note()
              << "they're sorted alphabetically -- this is GitHub's API being annoying, and I "
                 "can't fix it without an extra request per tag"
              << ansi::nl;
    return 0;
}

} // namespace cli::remote
