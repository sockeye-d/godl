#include "sizeconverter.h"

#include <QMap>

QString SizeConverter::formatSize(qint64 bytes, int decimalPlaces)
{
    static QList<QPair<qint64, QString>> units = {
        {1LL << 00, "B"},
        {1LL << 10, "KiB"},
        {1LL << 20, "MiB"},
        {1LL << 30, "GiB"},
        {1LL << 40, "TiB"},
    };

    qint64 selectedUnit = 1;
    QString selectedSuffix = "B";
    for (const auto &[unit, suffix] : units) {
        if (bytes > unit) {
            selectedUnit = unit;
            selectedSuffix = suffix;
        }
    }

    qint64 decimalPlace = std::pow(10, decimalPlaces + 1);
    return QString::number(static_cast<double>(bytes * decimalPlace / selectedUnit) / decimalPlace,
                           'f',
                           decimalPlaces)
           % " " % selectedSuffix;
}
