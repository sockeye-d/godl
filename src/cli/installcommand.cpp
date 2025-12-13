#include "installcommand.h"
#include "chainedjsonrequest.h"
#include "cli/ansi.h"
#include "cli/interface.h"
#include "downloadmanager.h"
#include "sizeconverter.h"
#include "versionregistry.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

using namespace Qt::StringLiterals;
using namespace std::chrono_literals;

namespace cli::install {

int install(const Parser &parser)
{
    const QString repo = parser.op("install").param("repo");
    const QString tag = parser.op("install").param("tag");
    const QString asset = parser.op("install").param("asset");
    const bool force = parser.set("force");
    auto bgv = new BoundGodotVersion();
    bgv->setRepo(repo);
    bgv->setTagName(tag);
    bgv->setIsMono(asset.contains("mono"));
    if (VersionRegistry::instance()->hasVersion(bgv)) {
        if (force) {
            qStdOut() << note() << "force-installing over previous installation" << ansi::nl;
        } else {
            qStdOut() << error() << "already downloaded" << ansi::nl;
            qStdOut() << note() << "override with --force" << ansi::nl;
            return 1;
        }
    }
    auto dl = new DownloadManager;
    auto req = new ChainedJsonRequest;
    auto del = qScopeGuard([dl, req]() {
        delete dl;
        delete req;
    });
    bool finished = false;
    QVariant fetchResult = QVariant();
    const auto &connection
        = req->connect(req,
                       &ChainedJsonRequest::finished,
                       req,
                       [&finished,                              // clazy:exclude=lambda-in-connect
                        &fetchResult](const QVariant &result) { // clazy:exclude=lambda-in-connect
                           fetchResult = result;
                           finished = true;
                       });
    req->execute(
        {u"%1/releases/tags/%2"_s.arg(VersionRegistry::instance()->resolveSourceUrl(repo), tag)});

    std::chrono::nanoseconds nowTime = std::chrono::system_clock::now().time_since_epoch();
    qStdOut() << ansi::cursorHide;
    qStdOut().flush();
    while (!finished) {
        QCoreApplication::processEvents();
        qStdOut() << ansi::cr << ansi::eraseInLine << progressBar(nowTime, "Fetching GitHub API");
        qStdOut().flush();
        QThread::sleep(10ms);
    }
    req->disconnect(connection);
    // qStdOut() << ansi::cursorShow;

    // print_debug() << fetchResult;

    QString dlUrl;
    for (const auto &arr = fetchResult.toList().constFirst().toMap().value("assets").toList();
         const auto &assetVar : arr) {
        if (assetVar.toMap().value("name").toString() == asset) {
            dlUrl = assetVar.toMap().value("browser_download_url").toString();
        }
    }

    qStdOut() << "\n";
    qStdOut().flush();

    if (dlUrl.isEmpty()) {
        qStdOut() << error() << "failed to find asset";
        return 1;
    }

    const DownloadInfo *dlInfo = dl->download(asset, tag, dlUrl, repo);
    if (!dlInfo || dlInfo->hasError()) {
        qStdOut() << error() << "failed to download\n";
        delete dlInfo;
        return 1;
    }

    nowTime = std::chrono::system_clock::now().time_since_epoch();
    while (!dlInfo->hasError() && dlInfo->stage() != DownloadInfo::Finished) {
        QCoreApplication::processEvents();
        QString caption;
        switch ((DownloadInfo::Stage) dlInfo->stage()) {
        case DownloadInfo::Downloading:
            caption = "Downloading";
            break;
        case DownloadInfo::Unzipping:
            caption = "Unzipping";
            break;
        case DownloadInfo::Finished:
            caption = "Finished downloading";
            break;
        case DownloadInfo::DownloadError:
        case DownloadInfo::UnzipError:
        case DownloadInfo::UnknownError:
            break;
        }
        caption += " " + asset;
        qStdOut() << ansi::cr << ansi::eraseInLine
                  << progressBar(nowTime,
                                 caption,
                                 dlInfo->progress(),
                                 u"{progress}% ⤓%1/s"_s.arg(SizeConverter::instance()->formatSize(
                                     dlInfo->downloadSpeed())));
        qStdOut().flush();
        QThread::sleep(10ms);
    }

    qStdOut() << "\n";
    qStdOut().flush();

    if (dlInfo->hasError()) {
        qStdOut() << error() << "download failed " << dlInfo->error();
        delete dlInfo;
        return 1;
    }

    qStdOut() << positive() << "done downloading " << asset << ansi::nl;

    // normally the user would do this
    delete dlInfo;
    QCoreApplication::processEvents();
    return 0;
}

} // namespace cli::install
