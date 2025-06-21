#include "downloadmanager.h"
#include <QDir>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QtConcurrent/QtConcurrent>
#include "macros.h"
#include "network.h"
#include "util/util.h"
#include <KArchive>
#include <KTar>
#include <KZip>
#include <config.h>

int countZipEntries(const KArchiveEntry *entry)
{
    if (!entry->isDirectory())
        return 1;
    int entries = 1;
    auto dir = (KArchiveDirectory *) entry;
    for (auto &subentryPath : dir->entries()) {
        qDebug() << subentryPath;
        entries += countZipEntries(dir->entry(subentryPath));
    }
    return entries;
}

void unzipDir(QPromise<void> &promise,
              const KArchiveDirectory *dir,
              QString destination,
              int entryCount)
{
    promise.finish();
}

void DownloadManager::unzip(DownloadInfo *info, QString sourceFilePath, QString destFilePath)
{
    info->setStage(DownloadInfo::Unzipping);
    auto archive = openArchive(sourceFilePath);
    if (!archive) {
        qDebug() << "Failed to open archive";
        model()->remove(info);
        return;
    }

    info->setProgress(-1.0);

    auto future = QtConcurrent::run(
        [archive = std::move(archive), destFilePath](QPromise<void> &promise) {
            int entries = countZipEntries(archive->directory());
            qDebug() << entries;
            unzipDir(promise, archive->directory(), destFilePath, entries);
        });

    // connect(
    //     this,
    //     &DownloadManager::cancellationRequested,
    //     this,
    //     [info, &future](QUuid id) {
    //         if (info->id() == id) {
    //             future.cancel();
    //         }
    //     },
    //     Qt::QueuedConnection);

    auto watcher = new QFutureWatcher<void>();
    connect(watcher, &QFutureWatcher<void>::finished, this, [info, watcher, this]() {
        model()->remove(info);
        watcher->deleteLater();
    });
    watcher->setFuture(future);
}

void DownloadManager::download(const QUrl &asset, const QString &assetName)
{
    auto downloadLocation = QStandardPaths::standardLocations(QStandardPaths::TempLocation)
                                .constFirst();
    auto path = downloadLocation / assetName;

    if (!QDir(downloadLocation).exists()) {
        qWarning() << u"Godot versions path doesn't exist, attempting to create...";
        if (!QDir().mkpath(downloadLocation)) {
            qCritical() << u"Failed to create godot path :(";
            return;
        }
    }

    auto info = new DownloadInfo(assetName, asset);
    m_model->append(info);

    qInfo() << u"Saving Godot version %1 from %2 at %3"_s.arg(assetName, asset.toString(), path);
    QFile *file = nullptr;
    if (QFile(path).exists()) {
        qInfo() << "Already found downloaded godot, not downloading";
        unzip(info, path, Config::godotLocation());
        return;
    } else {
        auto file = new QFile(path);
        if (!file->open(QFile::WriteOnly)) {
            qCritical() << u"Failed to open file at %1"_s.arg(path);
            file->deleteLater();
            return;
        }
    }

    QNetworkRequest request(asset);
    if (asset.host() == u"api.github.com"_s) {
        AUTH(request);
    }
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    request.setRawHeader("Connection", "close");

    QNetworkReply *const reply = Network::manager().get(request);

    reply->setReadBufferSize(128);

    file->setParent(reply);
    info->setParent(reply);

    auto time = new QElapsedTimer();
    time->start();
    auto bytesReceivedLast = new int;
    *bytesReceivedLast = 0;

    Q_EMIT downloadStarted();

    auto cancelConnection = connect(
        this,
        &DownloadManager::cancellationRequested,
        this,
        [info, reply](QUuid id) {
            if (info->id() == id) {
                reply->abort();
            }
        },
        Qt::QueuedConnection);

    connect(
        reply,
        &QNetworkReply::readyRead,
        this,
        [reply, file, info, time, bytesReceivedLast]() {
            auto data = reply->readAll();
            file->write(data);

            *bytesReceivedLast = *bytesReceivedLast + data.size();
            if (time->nsecsElapsed() > 100000000) {
                info->setDownloadSpeed(*bytesReceivedLast / (time->nsecsElapsed() * 1e-9)
                                       / 1048576.0);
                time->restart();
                *bytesReceivedLast = 0;
            }
        },
        Qt::QueuedConnection);

    connect(
        reply,
        &QNetworkReply::downloadProgress,
        this,
        [info](auto bytesReceived, auto bytesTotal) {
            info->setProgress((qreal) bytesReceived / (qreal) bytesTotal);
        },
        Qt::QueuedConnection);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [reply, info, time, bytesReceivedLast, path, cancelConnection, this]() {
            disconnect(cancelConnection);
            reply->deleteLater();
            delete time;
            delete bytesReceivedLast;
            unzip(info, path, Config::godotLocation());
        },
        Qt::QueuedConnection);
}
