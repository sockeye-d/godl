#ifndef VERSIONREGISTRYMODEL_H
#define VERSIONREGISTRYMODEL_H

#include <QAbstractListModel>
#include "godotversion.h"

class VersionRegistryModel : public QAbstractListModel
{
    Q_OBJECT

    QList<const GodotVersion *> m_data;

public:
    explicit VersionRegistryModel(QObject *parent = nullptr);

    void append(const GodotVersion *info);
    void remove(const GodotVersion *info);

    QHash<int, QByteArray> roleNames() const override { return {{Qt::UserRole + 1, "data"}}; }
    int rowCount(const QModelIndex &parent) const override
    {
        return parent.isValid() ? 0 : m_data.length();
    }
    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role == Qt::UserRole + 1) {
            return QVariant::fromValue(m_data[index.row()]);
        }
        return {};
    };
};

#endif // VERSIONREGISTRYMODEL_H
