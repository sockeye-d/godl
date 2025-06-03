//
// Created by fish on 5/31/25.
//

#include "chained_json_request.h"
#include "macros.h"
#include "main.h"
#include "util/util.h"
#include <QJsonDocument>
#include <QtLogging>
#include <qjsonobject.h>

QVariant ChainedJsonRequest::parse() const {
    QVariantList result;
    for (const auto &reply : replies) {
        result.append(QJsonDocument::fromJson(replyData[reply]).toVariant());
    }
    return result;
}

QVariant ChainedJsonRequest::parse(QNetworkReply *reply) const {
    return QJsonDocument::fromJson(replyData[reply]).toVariant();
}

void ChainedJsonRequest::execute(const QList<QUrl> &baseUrls) {
    currentTransformer = -1;
    cancel = false;
    replies.clear();
    executeInternal(baseUrls);
}

ChainedJsonRequest *ChainedJsonRequest::add(const QJSValue &transformer) {
    // if (transformer.isCallable()) {
    transformers.push_back([transformer](const QVariant &result) -> QVariant {
        return transformer.call({Main::engine->toScriptValue(result)}).toVariant();
    });
    // }
    return this;
}

void ChainedJsonRequest::executeInternal(const QList<QUrl> &baseUrls) {
    currentTransformer++;
    if (baseUrls.empty()) {
        const auto result = parse();
        Q_EMIT finished(result);
        // Q_EMIT finished(u"result"_s);
        return;
    }

    if (!replies.empty()) {
        for (auto reply : std::as_const(replies)) {
            reply->deleteLater();
        }
    }

    replies.clear();
    nextUrls.clear();
    replyData.clear();
    completedRequests = 0;
    totalRequests = baseUrls.size();

    for (auto &url : baseUrls) {
        QNetworkRequest request(url);
#ifdef GITHUB_TOKEN
        qDebug() << GITHUB_TOKEN;
        request.setRawHeader("Authorization"_ba,
                             u"Bearer %1"_s.arg(GITHUB_TOKEN).toUtf8());
#endif
        replies.append(nam.get(request));
    }

    for (QNetworkReply *reply : std::as_const(replies)) {
        connect(reply, &QNetworkReply::readyRead, this,
                [this, reply]() { replyData[reply].append(reply->readAll()); });

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (cancel) {
                return;
            }

            if (currentTransformer < transformers.size()) {
                auto result = transformers[currentTransformer](parse(reply));
                nextUrls.append(convertList<QUrl>(result.toList()));
            }
            completedRequests++;

            if (completedRequests >= totalRequests) {
                QList<QUrl> flattened;
                for (auto &url : nextUrls) {
                    flattened.append(url);
                }
                executeInternal(flattened);
            }
        });

        connect(reply, &QNetworkReply::errorOccurred, this,
                [this, reply](const QNetworkReply::NetworkError &networkError) {
            cancel = true;
            Q_EMIT error(currentTransformer, networkError,
                         reply->errorString());
        });
    }
}
