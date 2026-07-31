#include "downloadinfo.h"

void DownloadInfo::setError(const QString &error)
{
    if (m_error == error)
        return;
    m_error = error;
    Q_EMIT errorChanged();
}

void DownloadInfo::setProgress(const qint64 received, const qint64 total)
{
    if (received == m_received && total == m_total)
        return;
    m_received = received;
    m_total = total;
    Q_EMIT progressChanged();
}

void DownloadInfo::setStage(const Stage stage)
{
    if (m_stage == stage)
        return;
    m_stage = stage;
    Q_EMIT stageChanged();
}

void DownloadInfo::setDownloadSpeed(const qreal downloadSpeed)
{
    m_buf.append(downloadSpeed);
    m_downloadSpeed = std::accumulate(m_buf.begin(), m_buf.end(), 0.0) / m_buf.size();
    Q_EMIT downloadSpeedChanged();
}

DownloadInfo::DownloadInfo(QString assetName, QString tagName, QUrl sourceUrl, QString repo, QObject *parent)
    : QObject{parent}
    , m_assetName(std::move(assetName))
    , m_tagName(std::move(tagName))
    , m_sourceUrl(std::move(sourceUrl))
    , m_repo(std::move(repo))
{}

qreal DownloadInfo::progress() const
{
    return static_cast<qreal>(m_received) / static_cast<qreal>(m_total);
}

qint64 DownloadInfo::received() const
{
    return m_received;
}

qint64 DownloadInfo::total() const
{
    return m_total;
}

qreal DownloadInfo::downloadSpeed() const
{
    return m_downloadSpeed;
}

DownloadInfo::Stage DownloadInfo::stage() const
{
    return m_stage;
}

bool DownloadInfo::hasError() const
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

const QUuid &DownloadInfo::id() const
{
    return m_id;
}

const QString &DownloadInfo::assetName() const
{
    return m_assetName;
}

const QUrl &DownloadInfo::sourceUrl() const
{
    return m_sourceUrl;
}

const QString &DownloadInfo::error() const
{
    return m_error;
}

const QString &DownloadInfo::tagName() const
{
    return m_tagName;
}

const QString &DownloadInfo::repo() const
{
    return m_repo;
}