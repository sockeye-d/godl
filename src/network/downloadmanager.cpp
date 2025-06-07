#include "downloadmanager.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include "macros.h"
#include "network.h"

DownloadManager::DownloadManager(QObject *parent)
    : QStandardItemModel{parent}
{}

void DownloadManager::download(QUrl asset)
{
    QNetworkRequest request(asset);
    if (asset.host() == u"api.github.com"_s) {
        AUTH(request);
    }
    const auto reply = Network::manager().get(request);
    // const auto replyData = std::make_shared<QByteArray>();

    connect(reply, &QNetworkReply::finished, []() {

    });
}
