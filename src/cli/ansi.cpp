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

QString underline(const QString &string, int start, int end)
{
    if (end == -1) {
        end = string.size();
    } else if (end > string.size()) {
        end = string.size();
    }
    return string.sliced(0, start) + esc + "[4m" + string.sliced(start, end - start) + esc + "[24m"
           + string.sliced(end);
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

QTextStream &operator<<(QTextStream &left, TextStreamFlush)
{
    left.flush();
    return left;
}

QTextStream &operator<<(QTextStream &left, TextStreamFlushNl)
{
    left << cli::ansi::nl;
    left.flush();
    return left;
}
