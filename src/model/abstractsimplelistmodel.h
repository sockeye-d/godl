#ifndef ABSTRACTSIMPLELISTMODEL_H
#define ABSTRACTSIMPLELISTMODEL_H

#include <QAbstractListModel>

template<typename T, bool ptr = false>
class AbstractSimpleListModel : public QAbstractListModel
{
protected:
    QList<T> m_data;

public:
    using DataType = T;
    static constexpr int DataRole = Qt::UserRole + 1;

    explicit AbstractSimpleListModel(QObject *parent = nullptr)
        : QAbstractListModel{parent}
    {}

    void append(T data)
    {
        const auto index = m_data.size();

        beginInsertRows(QModelIndex(), index, index);
        m_data.append(data);
        endInsertRows();
    }

    void appendRange(const QList<T> &data)
    {
        const auto index = m_data.size();

        beginInsertRows(QModelIndex(), index, index + data.size() - 1);
        m_data.append(data);
        endInsertRows();
    }

    void remove(T data)
    {
        const auto index = m_data.indexOf(data);
        if (index == -1) {
            return;
        }
        beginRemoveRows(QModelIndex(), index, index);
        m_data.remove(index);
        endRemoveRows();
    }

    void clear()
    {
        beginRemoveRows(QModelIndex(), 0, m_data.size() - 1);
        m_data.clear();
        endRemoveRows();
    }

    QHash<int, QByteArray> roleNames() const override { return {{DataRole, "modelData"}}; }

    int rowCount(const QModelIndex &parent) const override
    {
        return parent.isValid() ? 0 : m_data.length();
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role == DataRole) {
            if constexpr (ptr) {
                static_assert(sizeof(qulonglong) >= sizeof(uintptr_t));
                return QVariant(reinterpret_cast<qulonglong>(&m_data[index.row()]));
            } else {
                return QVariant::fromValue(m_data[index.row()]);
            }
        }
        return {};
    };
};

template<typename T>
auto getDataFrom(const QPersistentModelIndex &index)
    requires(std::is_base_of_v<AbstractSimpleListModel<typename T::DataType, true>, T>)
{
    return reinterpret_cast<T::DataType *>(index.data(T::DataRole).toULongLong());
}

#endif // ABSTRACTSIMPLELISTMODEL_H
