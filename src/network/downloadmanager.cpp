#include "downloadmanager.h"
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include "macros.h"
#include "network.h"
#include <config.h>
#include <filesystem>

DownloadManager::DownloadManager(QObject *parent)
    : QStandardItemModel{parent}
{}

void DownloadManager::download(QUrl asset, QString tagName)
{
    QNetworkRequest request(asset);
    if (asset.host() == u"api.github.com"_s) {
        AUTH(request);
    }
    QNetworkReply *const reply = Network::manager().get(request);
    QByteArray *const replyData = new QByteArray();
    auto path = std::filesystem::path(Config::godotLocation().toStdString())
                / std::filesystem::path(tagName.toStdString());
    QScopedPointer file(new QFile(path));

    connect(reply, &QNetworkReply::readyRead, [reply, replyData]() {
        replyData->append(reply->readAll());
    });

    connect(reply, &QNetworkReply::finished, [reply]() { reply->deleteLater(); });
}
