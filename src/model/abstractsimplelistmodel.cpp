#include "abstractsimplelistmodel.h"

void AbstractSimpleListModel::append(void *data)
{
    const auto index = m_data.size();

    beginInsertRows(QModelIndex(), index, index);
    m_data.append(data);
    endInsertRows();
}

void AbstractSimpleListModel::remove(void *data)
{
    const auto index = m_data.indexOf(data);
    if (index == -1) {
        return;
    }
    beginRemoveRows(QModelIndex(), index, index);
    m_data.remove(index);
    endRemoveRows();
}

void AbstractSimpleListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_data.size() - 1);
    m_data.clear();
    endRemoveRows();
}

QHash<int, QByteArray> AbstractSimpleListModel::roleNames() const
{
    return {{DataRole, "modelData"}};
}

int AbstractSimpleListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_data.length();
}

QVariant AbstractSimpleListModel::data(const QModelIndex &index, int role) const
{
    if (role == DataRole) {
        return QVariant::fromValue(m_data[index.row()]);
    }
    return {};
}

QModelIndex AbstractSimpleListModel::index(int row, int column, const QModelIndex &parent) const
{
    return hasIndex(row, column, parent) ? createIndex(row, column) : QModelIndex();
}

QModelIndex AbstractSimpleListModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int AbstractSimpleListModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 1;
}
