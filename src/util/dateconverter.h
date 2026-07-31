#ifndef DATECONVERTER_H
#define DATECONVERTER_H

#include "singleton.h"

#include <QObject>

class DateConverter : public QObject, public Singleton<DateConverter>
{
    Q_OBJECT
public:
    explicit DateConverter(QObject *parent = nullptr);

    Q_INVOKABLE static QString relativeFormat(const QDateTime &date);
};

#endif // DATECONVERTER_H
