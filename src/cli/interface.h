#ifndef INTERFACE_H
#define INTERFACE_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <chrono>

double toSeconds(std::chrono::nanoseconds time);

namespace cli {

inline const int marqueeWidth = 32;
inline const int barWidth = 16;
inline const QStringList progressBarChars{"▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};
inline const QStringList reverseProgressBarChars{"█", "🮋", "🮊", "🮉", "▐", "🮈", "🮇", "▕"};
inline const QString progressBarEmptyChar = "█";
inline const QStringList indeterminateChars{
    {"█", "🮋", "🮊", "🮉", "▐", "🮈", "🮇", "▕", " ", "▏", "▎", "▍", "▌", "▋", "▊", "▉"},
};
inline const QStringList indeterminate{
    "⠇ ",
    "⠋ ",
    "⠉⠁",
    "⠈⠉",
    " ⠙",
    " ⠸",
    " ⢰",
    " ⣠",
    "⢀⣀",
    "⣀⡀",
    "⣄ ",
    "⡆ ",
};
inline const QString separator = " ";
inline const QMap<QString, bool> toggleValues{{"true", true},
                                              {"false", false},
                                              {"yes", true},
                                              {"no", false}};

QString progressBar(std::chrono::nanoseconds startTime,
                    const QString &caption,
                    double progress = -1.0,
                    const QString &rightCaption = "{progress}%");

QString asColumns(const QList<QStringList> &columns,
                  const QString &columnSeparator = " ",
                  bool equalColumnWidth = false);
QString prompt(const QString &display, const QStringList &allowedValues, int defaultValue = -1);
QString error();
QString info();
QString note();
QString positive();
bool parseBool(const QString &str, bool &out);
QString strBool(bool value);

} // namespace cli

#endif // INTERFACE_H
