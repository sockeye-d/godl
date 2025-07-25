#ifndef CHAINEDJSONREQUEST_H
#define CHAINEDJSONREQUEST_H

#include <QJSValue>
#include <QNetworkReply>
#include <QtQml/qqmlregistration.h>

using namespace Qt::Literals::StringLiterals;

class ChainedJsonRequest : public QObject {
public:
    typedef QVariant JsonTransformer(const QVariant &result, const QVariant &headers);

    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int running MEMBER m_running NOTIFY runningChanged)

    QList<QNetworkReply *> replies;
    std::vector<std::function<JsonTransformer>> transformers;
    size_t completedRequests = 0;
    size_t totalRequests = 0;
    size_t currentTransformer = -1;
    QMap<QNetworkReply *, QByteArray> replyData;
    QList<QUrl> nextUrls;
    bool cancel = false;

    QVariant parse() const;

    QVariant parse(QNetworkReply *reply) const;
    void executeInternal(const QList<QUrl> &baseUrls);
    bool m_running = false;
    void setRunning(bool running);

public:
    explicit ChainedJsonRequest(QObject *parent = nullptr) : QObject(parent) {}
    Q_INVOKABLE ChainedJsonRequest *add(const QJSValue &transformer);
    ChainedJsonRequest &addStep(std::function<JsonTransformer> transformer);

    Q_SIGNAL void error(size_t step, const QNetworkReply::NetworkError &error,
                        const QString &errorString);
    Q_SIGNAL void finished(const QVariant &result);
    Q_SIGNAL void runningChanged();

    Q_INVOKABLE Q_SLOT void execute(const QList<QUrl> &baseUrls);
};

#endif // CHAINEDJSONREQUEST_H
