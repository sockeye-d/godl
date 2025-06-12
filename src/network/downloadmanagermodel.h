#ifndef DOWNLOADMANAGERMODEL_H
#define DOWNLOADMANAGERMODEL_H

#include <QStandardItemModel>
#include <QUrl>
#include <QtQml/qqmlregistration.h>
#include "downloadinfo.h"

using namespace Qt::Literals::StringLiterals;

class DownloadManagerModel : public QAbstractListModel
{
    Q_OBJECT

    QList<const DownloadInfo *> m_dlInfos;

public:
    enum DownloadInfoRoles { ProgressRole = Qt::UserRole + 1, AssetNameRole, SourceUrlRole };

    void append(const DownloadInfo *info);
    void remove(const DownloadInfo *info);

public:
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override { return m_dlInfos.length(); }
    QVariant data(const QModelIndex &index, int role) const override;
};

#endif // DOWNLOADMANAGERMODEL_H
