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
#include "versionregistry.h"
#include <KArchive>
#include <KLocalization>
#include <KTar>
#include <KZip>
#include <config.h>

DownloadInfo *DownloadManager::createDlInfo(const QString &assetName,
                                            const QString &tagName,
                                            const QUrl &asset)
{
    auto info = new DownloadInfo(assetName, tagName, asset);
    connect(
        this,
        &DownloadManager::cancelRequested,
        this,
        [info, this](QUuid id) {
            if (info->id() == id) {
                m_model->remove(info);
                info->deleteLater();
            }
        },
        Qt::QueuedConnection);
    return info;
}

void DownloadManager::unzip(DownloadInfo *info, QString sourceFilePath, QString destFilePath)
{
    qDebug() << "Opening archive";
    info->setStage(DownloadInfo::Unzipping);
    info->setProgress(-1.0);

    auto future = QtConcurrent::run([sourceFilePath, destFilePath, info](
                                        QPromise<QString> &promise) {
        using namespace std::chrono_literals;
        QThread::sleep(500ms);
        auto archive = openArchive(sourceFilePath);
        if (!archive) {
            qDebug() << "Failed to open archive at " << sourceFilePath;
            info->setStage(DownloadInfo::UnzipError);
            info->setError(i18n("Failed to unzip archive"));
            promise.finish();
            return;
        }
        auto dest = destFilePath;
        qDebug() << archive->directory()->entries().size();
        if (archive->directory()->entries().size() == 1) {
            auto entryName = archive->directory()->entries().first();
            if (archive->directory()->entry(entryName)->isFile()) {
                qDebug() << "Single file zip";
                dest = QFileInfo(sourceFilePath).completeBaseName();
                QDir(destFilePath).mkpath(dest);
                dest = destFilePath / dest;
            }
        }
        qInfo() << "Copying file path to " << dest;
        if (!archive->directory()->copyTo(dest)) {
            info->setStage(DownloadInfo::UnzipError);
        } else {
            if (archive->directory()->entry(archive->directory()->entries().first())->isDirectory()) {
                dest = dest / archive->directory()->entries().first();
            }
            auto entries = QDir(dest).entryInfoList(QDir::Files);
            for (const QFileInfo &file : std::as_const(entries)) {
                qDebug() << file.canonicalFilePath();
                qDebug() << file.isExecutable();
                if (!file.fileName().contains("console")) {
                    promise.addResult(removePrefix(file.absoluteFilePath(),
                                                   normalizeDirectoryPath(destFilePath)));
                    break;
                }
            }
        }
        // QFile(sourceFilePath).remove();
        promise.finish();
    });

    auto watcher = new QFutureWatcher<QString>();
    connect(watcher,
            &QFutureWatcher<QString>::finished,
            this,
            [info, watcher, future, sourceFilePath]() {
                if (info->stage() != DownloadInfo::UnzipError) {
                    if (future.results().isEmpty()) {
                        info->setStage(DownloadInfo::UnknownError);
                        info->setError("Couldn't find Godot executable");
                    } else {
                        bool isMono = QFileInfo(sourceFilePath).completeBaseName().contains("mono");
                        VersionRegistry::instance().registerVersion(
                            std::make_shared<GodotVersion>(info->tagName(),
                                                           info->assetName(),
                                                           info->sourceUrl().toString(),
                                                           future.result(),
                                                           isMono));
                        info->setStage(DownloadInfo::Finished);
                    }
                }
                watcher->deleteLater();
            });
    watcher->setFuture(future);
}

void DownloadManager::download(const QString &assetName, const QString &tagName, const QUrl &asset)
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

    auto info = createDlInfo(assetName, tagName, asset);

    qInfo() << u"Saving Godot version %1 from %2 at %3"_s.arg(assetName, asset.toString(), path);
    auto file = new QFile(path);
    if (file->exists()) {
        qInfo() << "Already found downloaded godot, not downloading";
        m_model->append(info);
        unzip(info, path, Config::godotLocation());
        return;
    }

    if (!file->open(QIODeviceBase::WriteOnly | QIODeviceBase::Truncate)) {
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
    request.setRawHeader("Accept-Encoding", "identity");
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      "Mozilla/5.0 (X11; Linux x86_64; rv:139.0) Gecko/20100101 Firefox/139.0");

    QNetworkReply *const reply = Network::manager().get(request);

    auto time = new QElapsedTimer();
    time->start();
    auto bytesReceivedLast = new int;
    *bytesReceivedLast = 0;

    m_model->append(info);
    Q_EMIT downloadStarted();

    auto cancelConnection = connect(this,
                                    &DownloadManager::cancelRequested,
                                    this,
                                    [info, reply](QUuid id) {
                                        qDebug() << reply->headers().toMultiMap();
                                        if (info->id() == id) {
                                            reply->abort();
                                        }
                                    });

    connect(reply, &QNetworkReply::readyRead, this, [reply, file, info, time, bytesReceivedLast]() {
        auto data = reply->readAll();
        file->write(data);

        *bytesReceivedLast = *bytesReceivedLast + data.size();
        if (time->nsecsElapsed() > 100000000) {
            info->setDownloadSpeed(*bytesReceivedLast / (time->nsecsElapsed() * 1e-9) / 1048576.0);
            time->restart();
            *bytesReceivedLast = 0;
        }
    });

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
        [reply, info, time, bytesReceivedLast, path, cancelConnection, this, file]() {
            disconnect(cancelConnection);
            reply->deleteLater();
            file->close();
            file->deleteLater();
            delete time;
            delete bytesReceivedLast;
            if (reply->error() != QNetworkReply::NoError) {
                qWarning() << "Network reply failed with error " << reply->error() << " ("
                           << reply->errorString() << ")";
                QFile(path).remove();
                info->setStage(DownloadInfo::DownloadError);
                info->setError(reply->errorString());
            } else {
                unzip(info, path, Config::godotLocation());
            }
        },
        Qt::QueuedConnection);
}
