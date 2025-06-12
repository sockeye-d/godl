#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include <QStandardItemModel>
#include <QUrl>
#include <QUuid>
#include <QtQml/qqmlregistration.h>

using namespace Qt::Literals::StringLiterals;

class DownloadInfo : public QObject
{
    friend class DownloadManager;
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(qreal progress MEMBER m_progress NOTIFY progressChanged)
    Q_PROPERTY(qreal downloadSpeed MEMBER m_downloadSpeed NOTIFY downloadSpeedChanged)
    Q_PROPERTY(const QUuid id READ id CONSTANT)
    Q_PROPERTY(const QString assetName READ assetName CONSTANT)
    Q_PROPERTY(const QUrl sourceUrl READ sourceUrl CONSTANT)

    qreal m_progress = -1.0;
    qreal m_downloadSpeed = 0;
    QUuid m_id = QUuid::createUuid();
    const QString m_assetName;
    const QUrl m_sourceUrl;

    void setProgress(qreal progress)
    {
        if (qFuzzyCompare(progress, m_progress))
            return;
        m_progress = progress;
        Q_EMIT progressChanged();
    }

    void setDownloadSpeed(qreal downloadSpeed)
    {
        if (qFuzzyCompare(downloadSpeed, m_downloadSpeed))
            return;
        m_downloadSpeed = downloadSpeed;
        Q_EMIT downloadSpeedChanged();
    }

public:
    DownloadInfo(const QString &m_tagName, const QUrl &sourceUrl, QObject *parent = nullptr)
        : QObject{parent}
        , m_assetName(m_tagName)
        , m_sourceUrl(sourceUrl)
    {}

    bool operator==(DownloadInfo right);

    qreal progress() const { return m_progress; }
    qreal downloadSpeed() const { return m_downloadSpeed; }
    const QUuid id() const { return m_id; }

    const QString assetName() const { return m_assetName; }
    const QUrl sourceUrl() const { return m_sourceUrl; }

    Q_SIGNAL void progressChanged();
    Q_SIGNAL void downloadSpeedChanged();
};

#endif // DOWNLOADINFO_H
