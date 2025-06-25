#ifndef ICONCONVERTER_H
#define ICONCONVERTER_H

#include <QIcon>
#include <QObject>
#include <QQmlEngine>

class IconConverter : public QObject
{
    Q_OBJECT
    // QML_ELEMENT
    QML_SINGLETON
public:
    explicit IconConverter(QObject *parent = nullptr);

    Q_INVOKABLE QString name(QIcon icon) { return icon.name(); }
};

#endif // ICONCONVERTER_H
