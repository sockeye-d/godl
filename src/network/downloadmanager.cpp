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

void DownloadManager::unzip(DownloadInfo *info, QString sourceFilePath, QString destFilePath)
{
    using namespace std::chrono_literals;
    qDebug() << "Opening archive";
    info->setStage(DownloadInfo::Unzipping);
    auto archive = openArchive(sourceFilePath);
    info->setProgress(-1.0);

    if (!archive) {
        qDebug() << "Failed to open archive";
        m_model->remove(info);
        info->deleteLater();
        return;
    }

    auto future = QtConcurrent::run(
        [sourceFilePath, archive = std::move(archive), destFilePath](QPromise<bool> &promise) {
            auto dest = destFilePath;
            if (archive->directory()->entries().size() <= 1
                && archive->directory()->entry(archive->directory()->entries().first())->isFile()) {
                dest = getDirNameFromFilePath(sourceFilePath);
                QDir(destFilePath).mkpath(sourceFilePath.split(u"/"_s).last());
                dest = destFilePath / dest;
            }
            if (archive->directory()->copyTo(destFilePath)) {
                promise.addResult(true);
            } else {
                qDebug() << "Extraction failed";
                promise.addResult(false);
            }
            QFile(sourceFilePath).remove();
            QThread::sleep(250ms);
            promise.finish();
        });

    auto watcher = new QFutureWatcher<bool>();
    connect(watcher, &QFutureWatcher<bool>::finished, this, [info, watcher, this]() {
        m_model->remove(info);
        info->deleteLater();
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

    qInfo() << u"Saving Godot version %1 from %2 at %3"_s.arg(assetName, asset.toString(), path);
    auto file = new QFile(path);
    // if (file->exists()) {
    //     qInfo() << "Already found downloaded godot, not downloading";
    //     m_model->append(info);
    //     unzip(info, path, Config::godotLocation());
    //     return;
    // }

    if (!file->open(QFile::WriteOnly)) {
        qCritical() << u"Failed to open file at %1"_s.arg(path);
        file->deleteLater();
        info->deleteLater();
        return;
    }

    QNetworkRequest request(asset);
    if (asset.host() == u"api.github.com"_s) {
        AUTH(request);
    }
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    request.setRawHeader("Connection", "close");

    QNetworkReply *const reply = Network::manager().get(request);

    file->setParent(reply);

    auto time = new QElapsedTimer();
    time->start();
    auto bytesReceivedLast = new int;
    *bytesReceivedLast = 0;

    m_model->append(info);
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
