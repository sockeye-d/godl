#ifndef ANSI_H
#define ANSI_H

#include <QString>
#include <QTextStream>

class TextStreamFlush
{
    friend const TextStreamFlush &flush();
    TextStreamFlush() {}
};

class TextStreamFlushNl
{
    friend const TextStreamFlushNl &flushnl();
    TextStreamFlushNl() {}
};

inline const TextStreamFlush &flush()
{
    const static TextStreamFlush flush;
    return flush;
}

inline const TextStreamFlushNl &flushnl()
{
    const static TextStreamFlushNl flush;
    return flush;
}

QTextStream &qStdOut();
QTextStream &qStdIn();
QTextStream &operator<<(QTextStream &left, TextStreamFlush);
QTextStream &operator<<(QTextStream &left, TextStreamFlushNl);

namespace cli::ansi {
enum Direction {
    Up = 'A',
    Down = 'B',
    Right = 'C',
    Left = 'D',
};

enum Color {
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37,
    Default = 39,
    Reset = 0,
};

const inline QString esc = "\e";
const inline QString cr = "\r";
const inline QString nl = "\n";
const inline QString eraseInLine = esc + "[K";
const inline QString cursorHide = esc + "[?25l";
const inline QString cursorShow = esc + "[?25h";
const inline bool noColor = qgetenv("NO_COLOR") == "1";
const inline QString reset = esc + "[0m";

QString cursorMove(Direction direction, int count = 1);
QString fgcolor(Color color);
QString bgcolor(Color color);
QString underline(const QString &string, int start = 0, int end = -1);
} // namespace cli::ansi

#endif // ANSI_H
