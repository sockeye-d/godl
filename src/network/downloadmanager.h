#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QStandardItemModel>
#include <QTimer>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

#include "singleton.h"

class DownloadInfo;
class DownloadManagerModel;
using namespace Qt::Literals::StringLiterals;

class DownloadManager : public QObject, public Singleton<DownloadManager>
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(DownloadManagerModel *model READ model NOTIFY modelChanged FINAL)

    DownloadManagerModel *m_model;
    QTimer m_updateProgressTimer;
    int m_progressReceived{0};
    int m_progressTotal{0};

    Q_SIGNAL void cancelRequested(QUuid id);

    void unzip(DownloadInfo *info, const QString& sourceFilePath, const QString& destFilePath);
    DownloadInfo *createDlInfo(const QString &assetName, const QString &tagName, const QUrl &asset, const QString &repo);

private:
    [[nodiscard]] static QString getDownloadLocation(const DownloadInfo &info);
    void updateProgress();

public:
    explicit DownloadManager(QObject *parent = nullptr);

    Q_SIGNAL void modelChanged();
    Q_SIGNAL void downloadStarted();
    Q_SIGNAL void progressChanged(int progress, int progressTotal);

    [[nodiscard]] DownloadManagerModel *model() const;

    Q_INVOKABLE const DownloadInfo *download(const QString &assetName, const QString &tagName, const QUrl &asset, const QString &repo);
    Q_INVOKABLE void cancel(const QUuid &id);
    Q_INVOKABLE void remove(const QUuid &id) const;
};

#endif // DOWNLOADMANAGER_H
