#ifndef DATECONVERTER_H
#define DATECONVERTER_H

#include "singleton.h"

#include <QObject>
#include <qqmlintegration.h>

class DateConverter : public QObject, public Singleton<DateConverter>
{
    Q_OBJECT
    QML_SINGLETON
public:
    explicit DateConverter(QObject *parent = nullptr);

    Q_INVOKABLE QString relativeFormat(QDateTime date);
};

#endif // DATECONVERTER_H
