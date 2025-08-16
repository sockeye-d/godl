#ifndef SIZECONVERTER_H
#define SIZECONVERTER_H

#include "singleton.h"
#include <QObject>

class SizeConverter : public QObject, public Singleton<SizeConverter>
{
    Q_OBJECT
public:
    explicit SizeConverter(QObject *parent = nullptr)
        : QObject(parent) {};

    Q_INVOKABLE QString formatSize(qint64 bytes, int decimalPlaces = 2);
};

#endif // SIZECONVERTER_H
