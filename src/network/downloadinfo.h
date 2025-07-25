#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include <QStandardItemModel>
#include <QUrl>
#include <QUuid>
#include <QtQml/qqmlregistration.h>
#include "util/circularbuffer.h"

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
    Q_PROPERTY(int stage MEMBER m_stage NOTIFY stageChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged FINAL)
    Q_PROPERTY(QString repo READ repo NOTIFY repoChanged FINAL)

public:
    enum Stage {
        Downloading,
        Unzipping,
        Finished,
        DownloadError,
        UnzipError,
        UnknownError,
    };

    Q_ENUM(Stage)
private:
    qreal m_progress = -1.0;
    int m_stage = Downloading;
    qreal m_downloadSpeed = 0;
    bool m_hasError = false;
    const QUuid m_id = QUuid::createUuid();
    const QString m_assetName;
    const QString m_tagName;
    const QUrl m_sourceUrl;
    CircularBuffer<qreal, 128> m_buf;
    QString m_error = "";
    QString m_repo = "";

    void setError(QString error)
    {
        if (m_error == error)
            return;
        m_error = error;
        Q_EMIT errorChanged();
    }

    void setProgress(qreal progress)
    {
        if (qFuzzyCompare(progress, m_progress))
            return;
        m_progress = progress;
        Q_EMIT progressChanged();
    }

    void setStage(int stage)
    {
        if (m_stage == stage)
            return;
        m_stage = stage;
        Q_EMIT stageChanged();
    }

    void setDownloadSpeed(qreal downloadSpeed)
    {
        m_buf.append(downloadSpeed);
        m_downloadSpeed = std::accumulate(m_buf.begin(), m_buf.end(), 0.0) / m_buf.size();
        Q_EMIT downloadSpeedChanged();
    }

public:
    DownloadInfo(const QString assetName,
                 const QString tagName,
                 const QUrl sourceUrl,
                 const QString repo,
                 QObject *parent = nullptr)
        : QObject{parent}
        , m_assetName(assetName)
        , m_tagName(tagName)
        , m_sourceUrl(sourceUrl)
        , m_repo(repo)
    {}

    bool operator==(DownloadInfo right);

    qreal progress() const { return m_progress; }
    qreal downloadSpeed() const { return m_downloadSpeed; }
    int stage() const { return m_stage; }
    bool hasError() const
    {
        switch (stage()) {
        case DownloadError:
        case UnzipError:
        case UnknownError:
            return true;
        default:
            return false;
        }
    }
    const QUuid id() const { return m_id; }

    const QString assetName() const { return m_assetName; }
    const QUrl sourceUrl() const { return m_sourceUrl; }
    QString error() const { return m_error; }
    QString tagName() const { return m_tagName; }
    QString repo() const { return m_repo; }

    Q_SIGNAL void progressChanged();
    Q_SIGNAL void downloadSpeedChanged();
    Q_SIGNAL void stageChanged();
    Q_SIGNAL void errorChanged();
    Q_SIGNAL void repoChanged();
};

#endif // DOWNLOADINFO_H
