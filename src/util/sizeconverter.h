#ifndef SIZECONVERTER_H
#define SIZECONVERTER_H

#include "singleton.h"
#include <QObject>

namespace SizeConverter {
QString formatSize(qint64 bytes, int decimalPlaces = 2);
QString formatSize(int bytes, int decimalPlaces = 2);
QString formatSize(qreal bytes, int decimalPlaces = 2);
}

#endif // SIZECONVERTER_H
