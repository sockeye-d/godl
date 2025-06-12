#ifndef DOWNLOADINFO_H
#define DOWNLOADINFO_H

#include <QStandardItemModel>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

using namespace Qt::Literals::StringLiterals;

class DownloadInfo : public QObject
{
    friend class DownloadManager;
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(qreal progress MEMBER m_progress NOTIFY progressChanged)
    Q_PROPERTY(const QString *assetName READ assetName CONSTANT)
    Q_PROPERTY(const QUrl *sourceUrl READ sourceUrl CONSTANT)

    qreal m_progress = -1.0;
    const QString *m_assetName;
    const QUrl *m_sourceUrl;

    void setProgress(qreal progress)
    {
        if (qFuzzyCompare(progress, m_progress))
            return;
        m_progress = progress;
        Q_EMIT progressChanged();
    };

public:
    DownloadInfo(const QString *m_tagName, const QUrl *sourceUrl, QObject *parent = nullptr)
        : QObject{parent}
        , m_assetName(m_tagName)
        , m_sourceUrl(sourceUrl)
    {}

    qreal progress() const { return m_progress; }

    const QString *assetName() const { return m_assetName; }
    const QUrl *sourceUrl() const { return m_sourceUrl; }

    Q_SIGNAL void progressChanged();
};

#endif // DOWNLOADINFO_H
