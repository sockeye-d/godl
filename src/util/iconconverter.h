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
    static IconConverter *instance()
    {
        static auto instance = new IconConverter();
        return instance;
    }

    Q_INVOKABLE QString name(QIcon icon) { return icon.name(); }
    Q_INVOKABLE QUrl path(QString name, int size) const;
};

#endif // ICONCONVERTER_H
