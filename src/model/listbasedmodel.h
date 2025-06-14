#ifndef LISTBASEDMODEL_H
#define LISTBASEDMODEL_H

#include <QStandardItemModel>

class ListBasedModel : public QAbstractListModel
{
    typedef void Initializer(ListBasedModel &model, void *data);
    typedef void DataChangedSlot();
    Q_OBJECT

    QList<const void *> m_values;

    Initializer *const m_initializer;

    virtual QList<QByteArray> names() const { return {}; }

protected:
    virtual QVariant getData(const void *value, int role) const { return QVariant(); }

public:
    ListBasedModel(Initializer *initializer = nullptr)
        : m_initializer(initializer)
    {}

    void append(void *data)
    {
        beginInsertRows(QModelIndex(), m_values.size(), m_values.size());
        m_values.append(data);
        m_initializer(*this, data);
        endInsertRows();
    }

    bool remove(void *data)
    {
        int index = m_values.indexOf(data);
        if (index == -1) {
            return false;
        }
        beginRemoveRows(QModelIndex(), index, index);
        m_values.remove(index);
        endRemoveRows();
        return true;
    }

    QHash<int, QByteArray> roleNames() const override
    {
        auto names = QHash<int, QByteArray>();
        int i = 0;
        for (auto &name : this->names()) {
            names[i] = name;
            i++;
        }
        return names;
    }

    std::function<void()> bindChanged(QList<int> roles)
    {
        const void *lastData = m_values.last();
        return [&roles, lastData, this]() {
            auto index = createIndex(m_values.indexOf(lastData), 0);
            Q_EMIT dataChanged(index, index, roles);
        };
    }

    int rowCount(const QModelIndex &parent) const override { return m_values.size(); }
    QVariant data(const QModelIndex &index, int role) const override
    {
        return getData(m_values[index.row()], role);
    }
    explicit ListBasedModel(QObject *parent = nullptr);
};

#endif // LISTBASEDMODEL_H
