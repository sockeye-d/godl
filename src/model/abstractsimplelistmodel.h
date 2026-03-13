#ifndef ABSTRACTSIMPLELISTMODEL_H
#define ABSTRACTSIMPLELISTMODEL_H

#include <QAbstractItemModel>
#include <QtQmlIntegration/QtQmlIntegration>

class AbstractSimpleListModel : public QAbstractItemModel
{
    Q_OBJECT

protected:
    QList<void *> m_data;

public:
    static constexpr int DataRole = Qt::UserRole + 1;

    explicit AbstractSimpleListModel(QObject *parent = nullptr)
        : QAbstractItemModel{parent}
    {}

    void append(void *data);

    void remove(void *data);

    void clear();

    QHash<int, QByteArray> roleNames() const override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &) const override;

    int columnCount(const QModelIndex &parent) const override;
};

#endif // ABSTRACTSIMPLELISTMODEL_H
