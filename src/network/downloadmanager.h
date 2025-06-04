#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QStandardItemModel>
#include <QtQml/qqmlregistration.h>
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

class DownloadManager : public QStandardItemModel {
    Q_OBJECT
    QML_ELEMENT

    QList<DownloadInfo> m_items;

public:
    explicit DownloadManager(QObject *parent = nullptr);

    Q_INVOKABLE void download(QUrl asset);
};

#endif // DOWNLOADMANAGER_H
