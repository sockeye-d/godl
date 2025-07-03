#ifndef DATECONVERTER_H
#define DATECONVERTER_H

#include <QObject>
#include <qqmlintegration.h>

class DateConverter : public QObject
{
    Q_OBJECT
    QML_SINGLETON
public:
    static DateConverter *instance()
    {
        static auto instance = new DateConverter();
        return instance;
    }
    explicit DateConverter(QObject *parent = nullptr);

    Q_INVOKABLE QString relativeFormat(QDateTime date);
};

#endif // DATECONVERTER_H
