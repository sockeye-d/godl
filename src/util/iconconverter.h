#ifndef ICONCONVERTER_H
#define ICONCONVERTER_H

#include "singleton.h"

#include <QIcon>
#include <QObject>
#include <QQmlEngine>

class IconConverter : public QObject, public Singleton<IconConverter>
{
    Q_OBJECT
    // QML_ELEMENT
    QML_SINGLETON
public:
    explicit IconConverter(QObject *parent = nullptr);

    Q_INVOKABLE QString name(QIcon icon) { return icon.name(); }
    Q_INVOKABLE QUrl path(QString name, int size) const;
};

#endif // ICONCONVERTER_H
