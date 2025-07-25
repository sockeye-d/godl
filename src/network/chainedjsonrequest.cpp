//
// Created by fish on 5/31/25.
//

#include "chainedjsonrequest.h"
#include "macros.h"
#include "main.h"
#include "network.h"
#include "util/util.h"
#include <QJsonDocument>
#include <QtLogging>
#include <qjsonobject.h>

using namespace std::chrono_literals;

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
    setRunning(true);
    executeInternal(baseUrls);
}

ChainedJsonRequest *ChainedJsonRequest::add(const QJSValue &transformer) {
    // if (transformer.isCallable()) {
    transformers.push_back(
        [transformer](const QVariant &result, const QVariant &headers) -> QVariant {
            auto args = QJSValueList();
            args << Main::engine->toScriptValue(result);
            args << Main::engine->toScriptValue(headers);
            auto ret = transformer.call(args);
            return ret.toVariant();
        });
    // }
    return this;
}

ChainedJsonRequest &ChainedJsonRequest::addStep(std::function<JsonTransformer> transformer)
{
    transformers.push_back(transformer);
    return *this;
}

void ChainedJsonRequest::executeInternal(const QList<QUrl> &baseUrls) {
    currentTransformer++;
    if (baseUrls.empty()) {
        const auto result = parse();
        setRunning(false);
        Q_EMIT finished(result);
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
        AUTH(request);
        request.setRawHeader("X-GitHub-Api-Version"_ba, "2022-11-28"_ba);
        request.setTransferTimeout(5s);
        replies.append(Network::manager().get(request));
    }

    for (QNetworkReply *reply : std::as_const(replies)) {
        reply->setReadBufferSize(0);
        connect(reply, &QNetworkReply::readyRead, this, [this, reply]() {
            replyData[reply].append(reply->readAll());
        });

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (cancel) {
                return;
            }

            if (currentTransformer < transformers.size()) {
                auto headers = QVariantMap();
                for (auto &h : reply->headers().toListOfPairs()) {
                    headers[QString(h.first)] = QString(h.second);
                }

                auto result = transformers[currentTransformer](parse(reply), headers);
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

        connect(reply,
                &QNetworkReply::errorOccurred,
                this,
                [this, reply](const QNetworkReply::NetworkError &networkError) {
                    cancel = true;
                    setRunning(false);
                    Q_EMIT error(currentTransformer, networkError, reply->errorString());
                });
    }
}

void ChainedJsonRequest::setRunning(bool running)
{
    m_running = running;
    Q_EMIT runningChanged();
}
