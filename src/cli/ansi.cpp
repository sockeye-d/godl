#include "ansi.h"
#include <QDebug>

using namespace Qt::StringLiterals;

namespace cli::ansi {

QString cursorMove(Direction direction, int count)
{
    return u"%1[%2%3"_s.arg(esc).arg(count).arg(QString(static_cast<char>(direction)));
}

QString fgcolor(Color color)
{
    if (noColor) {
        return "";
    }
    return u"%1[%2m"_s.arg(esc).arg(color);
}

QString bgcolor(Color color)
{
    return fgcolor(static_cast<Color>(color + 10));
}

} // namespace cli::ansi

QTextStream &qStdOut()
{
    static QTextStream qts(stdout);
    return qts;
}

QTextStream &qStdIn()
{
    static QTextStream qts(stdin);
    return qts;
}
