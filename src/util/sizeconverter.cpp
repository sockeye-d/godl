#include "sizeconverter.h"

#include <QMap>

namespace {
qint64 pow10(const qint64 exponent)
{
    if (exponent < 0) {
        return 0;
    }

    switch (exponent) {
    case 0:
        return 1;
    case 1:
        return 10;
    case 2:
        return 100;
    case 3:
        return 1000;
    case 4:
        return 10000;
    case 5:
        return 100000;
    case 6:
        return 1000000;
    case 7:
        return 10000000;
    case 8:
        return 100000000;
    case 9:
        return 1000000000;
    case 10:
        return 10000000000;
    case 11:
        return 100000000000;
    case 12:
        return 1000000000000;
    case 13:
        return 10000000000000;
    case 14:
        return 100000000000000;
    case 15:
        return 1000000000000000;
    case 16:
        return 10000000000000000;
    case 17:
        return 100000000000000000;
    default:
        return 1000000000000000000;
    }
}
} // namespace

QString SizeConverter::formatSize(const qint64 bytes, const int decimalPlaces)
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

    const qint64 decimalPlace = pow10(decimalPlaces + 1);
    const double cursedExpression = static_cast<double>(bytes * decimalPlace / selectedUnit) // NOLINT(*-integer-division)
                                    / static_cast<double>(decimalPlace);
    return QString::number(cursedExpression, 'f', decimalPlaces) % " " % selectedSuffix;
}

QString SizeConverter::formatSize(const int bytes, const int decimalPlaces)
{
    return formatSize(static_cast<qint64>(bytes), decimalPlaces);
}

QString SizeConverter::formatSize(const qreal bytes, const int decimalPlaces)
{
    return formatSize(static_cast<qint64>(bytes), decimalPlaces);
}
