#ifndef VERSIONREGISTRYMODEL_H
#define VERSIONREGISTRYMODEL_H

#include <QAbstractListModel>
#include "godotversion.h"

class VersionRegistryModel : public QAbstractListModel
{
    Q_OBJECT

    QList<std::shared_ptr<const GodotVersion>> m_data;

public:
    static constexpr int DataRole = Qt::UserRole + 1;
    explicit VersionRegistryModel(QObject *parent = nullptr);

    void append(std::shared_ptr<const GodotVersion> info);
    void remove(std::shared_ptr<const GodotVersion> info);
    void clear();

    QHash<int, QByteArray> roleNames() const override { return {{DataRole, "modelData"}}; }
    int rowCount(const QModelIndex &parent) const override
    {
        return parent.isValid() ? 0 : m_data.length();
    }
    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role == DataRole) {
            return QVariant::fromValue(m_data[index.row()].get());
        }
        return {};
    };
};

#endif // VERSIONREGISTRYMODEL_H
