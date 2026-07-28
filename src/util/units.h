#ifndef UNITS_H
#define UNITS_H
#include <QApplication>
#include <QFont>

#include <qassert.h>

/**
 * https://invent.kde.org/frameworks/kirigami/-/blob/master/src/platform/units.cpp
 */
struct Units
{
    constexpr static int gridUnit = 18;
    constexpr static int smallSpacing = 4;
    constexpr static int mediumSpacing = 6;
    constexpr static int largeSpacing = 8;
    constexpr static int cornerRadius = 5;

    constexpr static int veryLongDuration = 400;
    constexpr static int longDuration = 200;
    constexpr static int shortDuration = 100;
    constexpr static int veryShortDuration = 50;
    constexpr static int humanMoment = 2000;
    constexpr static int toolTipDelay = 700;

    constexpr static double headingFactors[4] = {
        1.35,
        1.20,
        1.15,
        1.10,
    };

    static constexpr double headingFactor(const int level)
    {
        Q_ASSERT(level >= 1 && level <= 4);
        return headingFactors[level - 1];
    }

    static QFont heading(QFont font, const int level)
    {
        font.setPointSize(static_cast<int>(headingFactor(level) * QApplication::font().pointSize()));
        return font;
    }
};

#endif //UNITS_H
