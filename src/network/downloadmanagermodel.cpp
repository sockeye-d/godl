#include "downloadmanagermodel.h"

void DownloadManagerModel::append(const DownloadInfo *info)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_dlInfos.insert(0, info);
    Q_EMIT dataChanged(createIndex(0, 0),
                       createIndex(0, 0),
                       {
                           ProgressRole,
                           AssetNameRole,
                           SourceUrlRole,
                       });
    connect(info, &DownloadInfo::progressChanged, this, [this, info]() {
        QModelIndex index = createIndex(m_dlInfos.indexOf(info), 0);
        Q_EMIT dataChanged(index, index, {ProgressRole});
    });
    endInsertRows();
}

void DownloadManagerModel::remove(const DownloadInfo *info)
{
    const auto index = m_dlInfos.indexOf(info);
    beginRemoveRows(QModelIndex(), index, index);
    m_dlInfos.removeOne(info);
    endRemoveRows();
}

QHash<int, QByteArray> DownloadManagerModel::roleNames() const
{
    qDebug() << "getting role names";
    QHash<int, QByteArray> roles;
    roles[ProgressRole] = "progress";
    roles[AssetNameRole] = "assetName";
    roles[SourceUrlRole] = "sourceUrl";
    qDebug() << roles;
    return roles;
}

QVariant DownloadManagerModel::data(const QModelIndex &index, int role) const
{
    const auto info = m_dlInfos.at(index.row());
    switch (role) {
    case ProgressRole:
        return info->progress();
    case AssetNameRole:
        qDebug() << info->assetName();
        return *info->assetName();
    case SourceUrlRole:
        return *info->sourceUrl();
    }
    return {};
}
