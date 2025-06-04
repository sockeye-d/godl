#ifndef GODOTDOWNLOADMANAGER_H
#define GODOTDOWNLOADMANAGER_H

#include <QAbstractListModel>
#include <QJSValue>
#include <QObject>

using namespace Qt::Literals::StringLiterals;

class DownloadInfo : public QObject {
    Q_OBJECT

public:
    QVariant toVariant() const
    {
        QVariantMap map;
        map[u"url"_s] = QVariant(u"aaaa"_s);
        return map;
    }
};

class GodotDownloadManager : public QAbstractListModel {
    Q_OBJECT

    QList<DownloadInfo> m_items;

public:
    explicit GodotDownloadManager(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent)
        return m_items.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        return m_items[index.row()].toVariant();
    }

    Q_INVOKABLE void download(QJSValue asset);
};

#endif // GODOTDOWNLOADMANAGER_H
