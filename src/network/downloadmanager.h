#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QStandardItemModel>
#include <QUrl>
#include <QtQml/qqmlregistration.h>
#include "model/downloadmanagermodel.h"

using namespace Qt::Literals::StringLiterals;

class DownloadManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(DownloadManagerModel *model READ model NOTIFY modelChanged FINAL)

    DownloadManagerModel *m_model = new DownloadManagerModel();

    Q_SIGNAL void cancelRequested(QUuid id);

    void unzip(DownloadInfo *info, QString sourceFilePath, QString destFilePath);
    DownloadInfo *createDlInfo(const QString &assetName,
                               const QString &tagName,
                               const QUrl &asset,
                               const QString &repo);

public:
    explicit DownloadManager(QObject *parent = nullptr)
        : QObject(parent)
    {
        Q_EMIT modelChanged();
    }

    Q_SIGNAL void modelChanged();
    Q_SIGNAL void downloadStarted();

    DownloadManagerModel *model() { return m_model; }
    Q_INVOKABLE void download(const QString &assetName,
                              const QString &tagName,
                              const QUrl &asset,
                              const QString &repo);
    Q_INVOKABLE void cancel(const QUuid &id) { Q_EMIT cancelRequested(id); }
};

#endif // DOWNLOADMANAGER_H
