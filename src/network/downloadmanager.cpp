#include "downloadmanager.h"
#include <QDir>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include "macros.h"
#include "network.h"
#include "util/util.h"
#include <config.h>

void DownloadManager::download(const QUrl &asset, const QString &assetName)
{
    auto basePath = Config::godotLocation();
    auto path = joinPath(basePath, assetName);

    if (!QDir(basePath).exists()) {
        qCritical() << u"Godot versions path doesn't exist, attempting to create...";
        if (!QDir().mkpath(basePath)) {
            qCritical() << u"Failed to create godot path :(";
            return;
        }
    }

    qInfo() << u"Saving Godot version %1 from %2 at %3"_s.arg(assetName, asset.toString(), path);
    auto file = new QFile(path);
    if (!file->open(QFile::WriteOnly)) {
        qCritical() << u"Failed to open file at %1"_s.arg(path);
        file->deleteLater();
        return;
    }

    QNetworkRequest request(asset);
    if (asset.host() == u"api.github.com"_s) {
        AUTH(request);
    }

    QNetworkReply *const reply = Network::manager().get(request);
    auto info = new DownloadInfo(&assetName, &asset);
    m_model->append(info);

    Q_EMIT downloadStarted();

    connect(reply, &QNetworkReply::readyRead, this, [reply, file]() {
        file->write(reply->readAll());
    });
    connect(reply,
            &QNetworkReply::downloadProgress,
            this,
            [info](auto bytesReceived, auto bytesTotal) {
                info->setProgress((qreal) bytesReceived / (qreal) bytesTotal);
            });

    connect(reply, &QNetworkReply::finished, this, [reply, file, info, this]() {
        m_model->remove(info);
        reply->deleteLater();
        file->deleteLater();
        info->deleteLater();
    });
}
