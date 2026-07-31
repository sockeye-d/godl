#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include "util/circularbuffer.h"
#include <QStandardItemModel>
#include <QUrl>
#include <QUuid>

using namespace Qt::Literals::StringLiterals;

class DownloadInfo : public QObject
{
    friend class DownloadManager;
    friend class DownloadsPopup;

    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(qreal downloadSpeed MEMBER m_downloadSpeed NOTIFY downloadSpeedChanged)
    Q_PROPERTY(const QUuid id READ id CONSTANT)
    Q_PROPERTY(const QString assetName READ assetName CONSTANT)
    Q_PROPERTY(const QUrl sourceUrl READ sourceUrl CONSTANT)
    Q_PROPERTY(Stage stage MEMBER m_stage NOTIFY stageChanged)
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
    qint64 m_received = -1;
    qint64 m_total = -1;
    Stage m_stage = Downloading;
    qreal m_downloadSpeed = 0;
    bool m_hasError = false;
    const QUuid m_id = QUuid::createUuid();
    const QString m_assetName;
    const QString m_tagName;
    const QUrl m_sourceUrl;
    CircularBuffer<qreal, 128> m_buf;
    QString m_error = "";
    QString m_repo = "";

    void setError(const QString &error);
    void setProgress(qint64 received, qint64 total);
    void setStage(Stage stage);
    void setDownloadSpeed(qreal downloadSpeed);

public:
    DownloadInfo(QString assetName, QString tagName, QUrl sourceUrl, QString repo, QObject *parent = nullptr);

    [[nodiscard]] qreal progress() const;
    [[nodiscard]] qint64 received() const;
    [[nodiscard]] qint64 total() const;
    [[nodiscard]] qreal downloadSpeed() const;
    [[nodiscard]] Stage stage() const;
    [[nodiscard]] bool hasError() const;
    [[nodiscard]] const QUuid &id() const;
    [[nodiscard]] const QString &assetName() const;
    [[nodiscard]] const QUrl &sourceUrl() const;
    [[nodiscard]] const QString &error() const;
    [[nodiscard]] const QString &tagName() const;
    [[nodiscard]] const QString &repo() const;

    Q_SIGNAL void progressChanged();
    Q_SIGNAL void downloadSpeedChanged();
    Q_SIGNAL void stageChanged();
    Q_SIGNAL void errorChanged();
    Q_SIGNAL void repoChanged();
};

#endif // DOWNLOADINFO_H
