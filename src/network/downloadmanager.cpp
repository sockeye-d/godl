#include "downloadmanager.h"
#include "downloadmanagermodel.h"
#include "macros.h"
#include "network.h"
#include "registry/versionregistry.h"
#include "util/util.h"
#include <KArchive>
#include <config.h>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFutureWatcher>
#include <QMimeDatabase>
#include <QMimeType>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QUrl>
#include <QtConcurrentRun>

QString DownloadManager::getDownloadLocation(const DownloadInfo &info)
{
    return Config::godotLocation() / QUuid::createUuid().toString(QUuid::WithoutBraces) % "-"
           % VersionRegistry::instance()->sanitizeAssetName(info.assetName());
}

void DownloadManager::updateProgress()
{
    int progressReceived = -1;
    int progressTotal = -1;

    for (const auto &data : m_model->data()) {
        if (data->hasError()) {
            progressReceived = -2;
            break;
        }
        if (data->stage() == DownloadInfo::Finished && (progressReceived == -1 || progressReceived == -3)) {
            progressReceived = -3;
            continue;
        }
        if (progressReceived == -1) {
            progressReceived = 0;
            progressTotal = 0;
        }
        progressReceived += static_cast<int>(data->received());
        progressTotal += static_cast<int>(data->total());
    }

    if (progressReceived != m_progressReceived || progressTotal != m_progressTotal) {
        m_progressReceived = progressReceived;
        m_progressTotal = progressTotal;

        Q_EMIT progressChanged(progressReceived, progressTotal);
    }
}

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent)
    , m_model{new DownloadManagerModel()}
{
    Q_EMIT modelChanged();

    connect(&m_updateProgressTimer, &QTimer::timeout, this, &DownloadManager::updateProgress);
    m_updateProgressTimer.start(100);
    m_updateProgressTimer.setSingleShot(false);
}

DownloadInfo *DownloadManager::createDlInfo(const QString &assetName, const QString &tagName, const QUrl &asset, const QString &repo)
{
    auto info = new DownloadInfo(assetName, tagName, asset, repo);
    connect(
        this,
        &DownloadManager::cancelRequested,
        this,
        [info, this](const QUuid id) {
            if (info->id() == id) {
                m_model->remove(info);
                info->deleteLater();
            }
        },
        Qt::QueuedConnection);
    return info;
}

void DownloadManager::unzip(DownloadInfo *info, const QString &sourceFilePath, const QString &destFilePath)
{
    const QString configGroupName = QFileInfo(destFilePath).fileName();
    // TODO: clean up this spaghetti
    print_debug() << "Opening archive";
    info->setStage(DownloadInfo::Unzipping);
    info->setProgress(0, 0);

    auto future = QtConcurrent::run([sourceFilePath, destFilePath, info](QPromise<QString> &promise) {
        using namespace std::chrono_literals;
        const auto archive = openArchive(sourceFilePath);
        if (!archive) {
            if (const auto dl = QFileInfo(sourceFilePath);
                dl.isExecutable() || QMimeDatabase().mimeTypeForFile(sourceFilePath).inherits("application/x-executable")) {
                print_debug() << "Detected uncompressed executable";

#ifdef Q_OS_WIN
                QNtfsPermissionCheckGuard permissionGuard;
#endif
                if (!dl.isExecutable()) {
                    QFile(sourceFilePath).setPermissions(QFile::ExeOwner | dl.permissions());
                }

                const auto destName = dl.fileName();
                const auto path = destFilePath / destName / dl.fileName();
                if (!QDir(destFilePath).mkpath(destName)) {
                    qFatal() << "Failed making path" << destName;
                }
                if (!QFile::copy(sourceFilePath, path)) {
                    print_debug() << "failed to copy executable";
                }
                promise.addResult(removePrefix(path, normalizeDirectoryPath(VersionRegistry::instance()->locationDirectory())));

                if (!Config::cacheVersions())
                    QFile(sourceFilePath).remove();

                promise.finish();
                return;
            } else {
                print_debug() << "Failed to open archive at " << sourceFilePath;
                info->setStage(DownloadInfo::UnzipError);
                info->setError("Failed to unzip archive");
                promise.finish();
                return;
            }
        }
        auto dest = destFilePath;
        print_debug() << archive->directory()->entries().size();
        print_debug() << archive->directory()->name();
        if (archive->directory()->entries().size() != 1 || archive->directory()->entry(archive->directory()->entries().at(0))->isFile()) {
            print_debug() << "Cursed zip, extracting to folder";
            dest = QFileInfo(sourceFilePath).completeBaseName();
            if (!QDir(destFilePath).mkpath(dest)) {
                qFatal() << "Failed making path" << dest;
            }
            dest = destFilePath / dest;
        }
        qInfo() << "Copying file path to " << dest;
        if (!archive->directory()->copyTo(dest)) {
            info->setStage(DownloadInfo::UnzipError);
        } else {
            if (archive->directory()->entry(archive->directory()->entries().first())->isDirectory()
                && archive->directory()->entries().size() == 1) {
                dest = dest / archive->directory()->entries().first();
            }
            for (auto entries = QDir(dest).entryInfoList(QDir::Files | QDir::Executable); const QFileInfo &file : std::as_const(entries)) {
                print_debug() << file.canonicalFilePath();
                print_debug() << file.isExecutable();
                if (!file.fileName().contains("console")) {
                    promise.addResult(
                        removePrefix(file.absoluteFilePath(), normalizeDirectoryPath(VersionRegistry::instance()->locationDirectory())));
                    break;
                }
            }
        }

        if (!Config::cacheVersions())
            QFile(sourceFilePath).remove();

        promise.finish();
    });

    auto watcher = new QFutureWatcher<QString>();
    connect(watcher, &QFutureWatcher<QString>::finished, this, [info, watcher, future, sourceFilePath, configGroupName]() {
        if (info->stage() != DownloadInfo::UnzipError) {
            if (future.results().isEmpty()) {
                info->setStage(DownloadInfo::UnknownError);
                info->setError("Couldn't find Godot executable");
            } else {
                const bool isMono = QFileInfo(sourceFilePath).completeBaseName().contains("mono");
                VersionRegistry::instance()->registerVersion(new GodotVersion(configGroupName,
                                                                              info->tagName(),
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

DownloadManagerModel *DownloadManager::model() const
{
    return m_model;
}

static void connectUpInfo(QAbstractItemModel *model, const DownloadInfo *info, const QPersistentModelIndex &index)
{
    const auto target = [model, index]() {
        const QModelIndex weakIndex = model->index(index.row(), 0);
        Q_EMIT model->dataChanged(weakIndex, weakIndex);
    };

    QObject::connect(info, &DownloadInfo::progressChanged, model, target);
    QObject::connect(info, &DownloadInfo::downloadSpeedChanged, model, target);
    QObject::connect(info, &DownloadInfo::stageChanged, model, target);
    QObject::connect(info, &DownloadInfo::errorChanged, model, target);
    QObject::connect(info, &DownloadInfo::repoChanged, model, target);
};

const DownloadInfo *DownloadManager::download(const QString &assetName, const QString &tagName, const QUrl &asset, const QString &repo)
{
    const auto downloadLocation = QStandardPaths::standardLocations(QStandardPaths::TempLocation).constFirst();
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
        connectUpInfo(m_model, info, m_model->append<QPersistentModelIndex>(info));
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
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (X11; Linux x86_64; rv:139.0) Gecko/20100101 Firefox/139.0");

    QNetworkReply *const reply = Network::manager().get(request);

    auto time = new QElapsedTimer();
    time->start();
    auto bytesReceivedLast = new qsizetype;
    *bytesReceivedLast = 0;

    connectUpInfo(m_model, info, m_model->append<QPersistentModelIndex>(info));
    Q_EMIT downloadStarted();

    auto cancelConnection = connect(this, &DownloadManager::cancelRequested, this, [info, reply](const QUuid &id) {
        if (info->id() == id) {
            reply->abort();
        }
    });

    connect(reply, &QNetworkReply::readyRead, this, [reply, file, info, time, bytesReceivedLast]() {
        const auto data = reply->readAll();
        file->write(data);

        *bytesReceivedLast += data.size();
        if (time->nsecsElapsed() > 100000000) {
            info->setDownloadSpeed(static_cast<double>(*bytesReceivedLast) / (static_cast<double>(time->nsecsElapsed()) * 1e-9));
            time->restart();
            *bytesReceivedLast = 0;
        }
    });

    connect(
        reply,
        &QNetworkReply::downloadProgress,
        this,
        [info](const qint64 bytesReceived, const qint64 bytesTotal) { info->setProgress(bytesReceived, bytesTotal); },
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
                qWarning() << "Network reply failed with error " << reply->error() << " (" << reply->errorString() << ")";
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

void DownloadManager::cancel(const QUuid &id)
{
    Q_EMIT cancelRequested(id);
}

void DownloadManager::remove(const QUuid &id) const
{
    DownloadInfo *foundInfo = nullptr;
    print_debug() << model()->m_data;
    for (DownloadInfo *info : std::as_const(model()->m_data)) {
        if (info->id() == id) {
            foundInfo = info;
            break;
        }
    }

    if (foundInfo == nullptr) {
        print_debug() << "Failed to find download info\n";
        return;
    }

    model()->remove(foundInfo);
    foundInfo->deleteLater();
}
