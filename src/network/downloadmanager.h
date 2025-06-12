#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QStandardItemModel>
#include <QUrl>
#include <QtQml/qqmlregistration.h>
#include "downloadmanagermodel.h"

using namespace Qt::Literals::StringLiterals;

class DownloadManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(DownloadManagerModel *model READ model NOTIFY modelChanged FINAL)

    DownloadManagerModel *m_model = new DownloadManagerModel();

    Q_SIGNAL void cancellationRequested(QUuid id);

public:
    explicit DownloadManager(QObject *parent = nullptr)
        : QObject(parent)
    {
        Q_EMIT modelChanged();
    }

    Q_SIGNAL void modelChanged();
    Q_SIGNAL void downloadStarted();

    DownloadManagerModel *model() { return m_model; }
    Q_INVOKABLE void download(const QUrl &asset, const QString &assetName);
    Q_INVOKABLE void cancel(const QUuid &id) { Q_EMIT cancellationRequested(id); }
};

#endif // DOWNLOADMANAGER_H
