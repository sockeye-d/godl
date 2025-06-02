//
// Created by fish on 5/31/25.
//

#include "chained_json_request.h"

#include "macros.h"

QList<QJsonDocument> ChainedJsonRequest::parse() const {
    QList<QJsonDocument> result;
    for (const auto &reply: replies) {
        result.append(QJsonDocument::fromJson(replyData[reply]));
    }
    return result;
}

QJsonDocument ChainedJsonRequest::parse(QNetworkReply *reply) const {
    return QJsonDocument::fromJson(replyData[reply]);
}

void ChainedJsonRequest::execute(const QList<QUrl> &baseUrls) {
    currentTransformer = -1;
    cancel = false;
    replies.clear();
    executeInternal(baseUrls);
}

ChainedJsonRequest *ChainedJsonRequest::add(JsonTransformer *transformer) {
    transformers.push_back(transformer);
    return this;
}

void ChainedJsonRequest::executeInternal(const QList<QUrl> &baseUrls) {
    currentTransformer++;
    if (baseUrls.empty()) {
        const auto result = parse();
        Q_EMIT finished(result);
        return;
    }

    if (!replies.empty()) {
        for (auto reply: std::as_const(replies)) {
            reply->deleteLater();
        }
    }

    replies.clear();
    nextUrls.clear();
    replyData.clear();
    completedRequests = 0;
    totalRequests = baseUrls.size();

    for (auto &url: baseUrls) {
        QNetworkRequest request(url);
#ifdef GITHUB_TOKEN
        QTextStream(stdout) << u"hi"_s;
        request.setRawHeader("Authorization"_ba, u"Bearer %1"_s.arg(token(GITHUB_TOKEN)).toUtf8());
#endif
        replies.append(nam.get(request));
    }

    for (QNetworkReply *reply: std::as_const(replies)) {
        connect(reply, &QNetworkReply::readyRead, this, [this, reply]() {
            replyData[reply].append(reply->readAll());
        });

        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (cancel) {
                return;
            }

            if (currentTransformer < transformers.size())
                nextUrls.append(transformers[currentTransformer](parse(reply)));
            completedRequests++;

            if (completedRequests >= totalRequests) {
                QList<QUrl> flattened;
                for (auto &url: nextUrls) {
                    flattened.append(url);
                }
                executeInternal(flattened);
            }
        });

        connect(reply, &QNetworkReply::errorOccurred, this,
                [this, reply](const QNetworkReply::NetworkError &networkError) {
                    cancel = true;
                    Q_EMIT error(currentTransformer, networkError, reply->errorString());
                }
        );
    }
}
