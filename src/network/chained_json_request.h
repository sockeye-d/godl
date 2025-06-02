//
// Created by fish on 5/31/25.
//

#ifndef CHAINED_JSON_REQUEST_H
#define CHAINED_JSON_REQUEST_H
#include <QNetworkReply>
#include <QJsonDocument>
#include <QtQml/qqmlregistration.h>

using namespace Qt::Literals::StringLiterals;

class ChainedJsonRequest : public QObject {
    typedef QList<QUrl> JsonTransformer(const QJsonDocument &result);

    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int running MEMBER m_running)

    QList<QNetworkReply*> replies;
    std::vector<JsonTransformer *> transformers;
    size_t completedRequests = 0;
    size_t totalRequests = 0;
    size_t currentTransformer = -1;
    QMap<QNetworkReply*, QByteArray> replyData;
    QList<QUrl> nextUrls;
    QNetworkAccessManager nam;
    bool cancel = false;

    QList<QJsonDocument> parse() const;

    QJsonDocument parse(QNetworkReply *reply) const;
    void executeInternal(const QList<QUrl> &baseUrls);
    bool m_running = false;

public:

    explicit ChainedJsonRequest(QObject *parent = nullptr) : QObject(parent) {}
    ChainedJsonRequest *add(JsonTransformer *transformer);

    Q_SIGNAL void error(size_t step, const QNetworkReply::NetworkError &error, const QString &errorString);
    Q_SIGNAL void finished(const QList<QJsonDocument> &result);

    Q_INVOKABLE Q_SLOT void execute(const QList<QUrl> &baseUrls);
};


#endif //CHAINED_JSON_REQUEST_H
