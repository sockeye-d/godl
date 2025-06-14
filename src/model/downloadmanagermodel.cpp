#include "downloadmanagermodel.h"

void DownloadManagerModel::append(const DownloadInfo *info)
{
    const auto index = m_dlInfos.size();
    beginInsertRows(QModelIndex(), index, index);
    m_dlInfos.append(info);
    connect(info, &DownloadInfo::progressChanged, this, [this, info]() {
        QModelIndex index = createIndex(m_dlInfos.indexOf(info), 0);
        Q_EMIT dataChanged(index, index, {ProgressRole});
    });
    connect(info, &DownloadInfo::downloadSpeedChanged, this, [this, info]() {
        QModelIndex index = createIndex(m_dlInfos.indexOf(info), 0);
        Q_EMIT dataChanged(index, index, {DownloadSpeedRole});
    });
    endInsertRows();
}

void DownloadManagerModel::remove(const DownloadInfo *info)
{
    const auto index = m_dlInfos.indexOf(info);
    beginRemoveRows(QModelIndex(), index, index);
    m_dlInfos.remove(index);
    endRemoveRows();
}

QHash<int, QByteArray> DownloadManagerModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ProgressRole] = "progress";
    roles[AssetNameRole] = "assetName";
    roles[SourceUrlRole] = "sourceUrl";
    roles[IdRole] = "id";
    roles[DownloadSpeedRole] = "downloadSpeed";
    return roles;
}

QVariant DownloadManagerModel::data(const QModelIndex &index, int role) const
{
    const auto info = m_dlInfos.at(index.row());
    switch (role) {
    case ProgressRole:
        return info->progress();
    case AssetNameRole:
        return info->assetName();
    case SourceUrlRole:
        return info->sourceUrl();
    case IdRole:
        return info->id();
    case DownloadSpeedRole:
        return info->downloadSpeed();
    }
    return {};
}
