#include "versionregistrymodel.h"

VersionRegistryModel::VersionRegistryModel(QObject *parent)
    : QAbstractListModel{parent}
{}

void VersionRegistryModel::append(const GodotVersion *info)
{
    const auto index = m_data.size();

    beginInsertRows(QModelIndex(), index, index);
    m_data.append(info);
    endInsertRows();
}

void VersionRegistryModel::remove(const GodotVersion *info)
{
    const auto index = m_data.indexOf(info);
    beginRemoveRows(QModelIndex(), index, index);
    m_data.remove(index);
    endRemoveRows();
}
