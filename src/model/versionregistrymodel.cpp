#include "versionregistrymodel.h"

VersionRegistryModel::VersionRegistryModel(QObject *parent)
    : QAbstractListModel{parent}
{}

void VersionRegistryModel::append(std::shared_ptr<const GodotVersion> info)
{
    const auto index = m_data.size();

    beginInsertRows(QModelIndex(), index, index);
    m_data.append(info);
    endInsertRows();
}

void VersionRegistryModel::remove(std::shared_ptr<const GodotVersion> info)
{
    const auto index = m_data.indexOf(info);
    beginRemoveRows(QModelIndex(), index, index);
    m_data.remove(index);
    endRemoveRows();
}

void VersionRegistryModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_data.size() - 1);
    m_data.clear();
    endRemoveRows();
}
