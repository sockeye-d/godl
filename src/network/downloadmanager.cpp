#include "downloadmanager.h"
#include <QDir>
#include <QFile>
#include <QFutureWatcher>
#include <QMimeDatabase>
#include <QMimeType>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNtfsPermissionCheckGuard>
#include <QRegularExpression>
#include <QUrl>
#include <QtConcurrentRun>
#include "macros.h"
#include "network.h"
#include "registry/versionregistry.h"
#include "util/util.h"
#include <KArchive>
#include <KLocalization>
#include <KTar>
#include <KZip>
#include <config.h>

namespace {
QString sanitize(QString input, const QString &withWhat = "_")
{
    static QRegularExpression re{"[<!>:\"\\/\\|?*]", QRegularExpression::MultilineOption};
    return input.replace(re, withWhat);
}

QString getDownloadLocation(const DownloadInfo &info)
{
    return Config::godotLocation() / QUuid::createUuid().toString(QUuid::WithoutBraces) % "-"
           % sanitize(info.assetName());
}
} // namespace

DownloadInfo *DownloadManager::createDlInfo(const QString &assetName,
                                            const QString &tagName,
                                            const QUrl &asset,
                                            const QString &repo)
{
    auto info = new DownloadInfo(assetName, tagName, asset, repo);
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
    // TODO: clean up this spaghetti
    print_debug() << "Opening archive";
    info->setStage(DownloadInfo::Unzipping);
    info->setProgress(-1.0);

    auto future = QtConcurrent::run([sourceFilePath, destFilePath, info](
                                        QPromise<QString> &promise) {
        using namespace std::chrono_literals;
        auto archive = openArchive(sourceFilePath);
        if (!archive) {
            const auto dl = QFileInfo(sourceFilePath);
            if (dl.isExecutable()
                || QMimeDatabase()
                       .mimeTypeForFile(sourceFilePath)
                       .inherits("application/x-executable")) {
                print_debug() << "Detected uncompressed executable";

#ifdef Q_OS_WIN
                QNtfsPermissionCheckGuard permissionGuard;
#endif
                if (!dl.isExecutable()) {
                    QFile(sourceFilePath).setPermissions(QFile::ExeOwner | dl.permissions());
                }

                const auto destName = dl.fileName();
                const auto path = destFilePath / destName / dl.fileName();
                QDir(destFilePath).mkpath(destName);
                if (!QFile::copy(sourceFilePath, path)) {
                    print_debug() << "failed to copy executable";
                }
                promise.addResult(destName / dl.fileName());

                if (!Config::cacheVersions())
                    QFile(sourceFilePath).remove();

                promise.finish();
                return;
            } else {
                print_debug() << "Failed to open archive at " << sourceFilePath;
                info->setStage(DownloadInfo::UnzipError);
                info->setError(i18n("Failed to unzip archive"));
                promise.finish();
                return;
            }
        }
        auto dest = destFilePath;
        print_debug() << archive->directory()->entries().size();
        print_debug() << archive->directory()->name();
        if (archive->directory()->entries().size() != 1
            || archive->directory()->entry(archive->directory()->entries().at(0))->isFile()) {
            print_debug() << "Cursed zip, extracting to folder";
            dest = QFileInfo(sourceFilePath).completeBaseName();
            QDir(destFilePath).mkpath(dest);
            dest = destFilePath / dest;
        }
        qInfo() << "Copying file path to " << dest;
        if (!archive->directory()->copyTo(dest)) {
            info->setStage(DownloadInfo::UnzipError);
        } else {
            QThread::sleep(200ms);
            if (archive->directory()->entry(archive->directory()->entries().first())->isDirectory()
                && archive->directory()->entries().size() == 1) {
                dest = dest / archive->directory()->entries().first();
            }
            auto entries = QDir(dest).entryInfoList(QDir::Files | QDir::Executable);
            for (const QFileInfo &file : std::as_const(entries)) {
                print_debug() << file.canonicalFilePath();
                print_debug() << file.isExecutable();
                if (!file.fileName().contains("console")) {
                    promise.addResult(
                        removePrefix(file.absoluteFilePath(),
                                     normalizeDirectoryPath(
                                         VersionRegistry::instance()->locationDirectory())));
                    break;
                }
            }
        }

        if (!Config::cacheVersions())
            QFile(sourceFilePath).remove();

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
                        VersionRegistry::instance()->registerVersion(
                            new GodotVersion(info->tagName(),
                                             info->assetName(),
                                             info->sourceUrl().toString(),
                                             info->repo(),
                                             future.result(),
                                             isMono));
                        info->setStage(DownloadInfo::Finished);
                    }
                }
                watcher->deleteLater();
            });
    watcher->setFuture(future);
}

const DownloadInfo *DownloadManager::download(const QString &assetName,
                                              const QString &tagName,
                                              const QUrl &asset,
                                              const QString &repo)
{
    auto downloadLocation = QStandardPaths::standardLocations(QStandardPaths::TempLocation)
                                .constFirst();
    auto path = downloadLocation / assetName;

    if (!QDir(downloadLocation).exists()) {
        qWarning() << u"Godot versions path doesn't exist, attempting to create...";
        if (!QDir().mkpath(downloadLocation)) {
            qCritical() << u"Failed to create godot path :(";
            return nullptr;
        }
    }

    auto info = createDlInfo(assetName, tagName, asset, repo);

    qInfo() << u"Saving Godot version %1 from %2 at %3"_s.arg(assetName, asset.toString(), path);
    auto file = new QFile(path);
    if (file->exists() && Config::cacheVersions()) {
        qInfo() << "Already found downloaded godot, not downloading";
        m_model->append(info);
        Q_EMIT downloadStarted();
        unzip(info, path, getDownloadLocation(*info));
        return info;
    }

    if (!file->open(QIODeviceBase::WriteOnly | QIODeviceBase::Truncate)) {
        qCritical() << u"Failed to open file at %1"_s.arg(path);
        file->deleteLater();
        info->deleteLater();
        return nullptr;
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
                                        print_debug() << reply->headers().toMultiMap();
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
                unzip(info, path, getDownloadLocation(*info));
            }
        },
        Qt::QueuedConnection);

    return info;
}
