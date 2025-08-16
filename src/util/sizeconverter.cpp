#include "sizeconverter.h"

#include <QMap>

QString SizeConverter::formatSize(qint64 bytes, int decimalPlaces)
{
    static QMap<qint64, QString> units = {
        {1LL << 00, "B"},
        {1LL << 10, "KiB"},
        {1LL << 20, "MiB"},
        {1LL << 30, "GiB"},
        {1LL << 40, "TiB"},
    };

    qint64 selectedUnit = 1;
    QString selectedSuffix = "B";
    for (auto unit : units.asKeyValueRange()) {
        if (bytes > unit.first) {
            selectedUnit = unit.first;
            selectedSuffix = unit.second;
        }
    }

    qint64 decimalPlace = std::pow(10, decimalPlaces + 1);
    return QString::number(double(bytes * decimalPlace / selectedUnit) / decimalPlace,
                           'f',
                           decimalPlaces)
           % " " % selectedSuffix;
}
