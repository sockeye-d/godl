#ifndef ABSTRACTSIMPLELISTMODEL_H
#define ABSTRACTSIMPLELISTMODEL_H

#include <QAbstractListModel>

template<typename T>
class AbstractSimpleListModel : public QAbstractListModel
{
protected:
    QList<T> m_data;

public:
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
    void remove(T data)
    {
        const auto index = m_data.indexOf(data);
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
            return QVariant::fromValue(m_data[index.row()]);
        }
        return {};
    };
};

#endif // ABSTRACTSIMPLELISTMODEL_H
