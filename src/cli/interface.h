#ifndef INTERFACE_H
#define INTERFACE_H

#include <QMap>
#include <QString>
#include <QStringList>
#include "godotversion.h"
#include <chrono>

double toSeconds(std::chrono::nanoseconds time);

namespace cli {

inline const int marqueeWidth = 32;
inline const QString marqueeElideString = "…";
inline const int barWidth = 16;
inline const QStringList progressBarChars{"▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};
inline const QStringList reverseProgressBarChars{"█", "🮋", "🮊", "🮉", "▐", "🮈", "🮇", "▕"};
inline const QString progressBarEmptyChar = "█";
inline const QStringList indeterminateChars{
    {"█", "🮋", "🮊", "🮉", "▐", "🮈", "🮇", "▕", " ", "▏", "▎", "▍", "▌", "▋", "▊", "▉"},
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

using Table = QList<QStringList>;
QString asColumns(const Table &table,
                  const QString &columnSeparator = " ",
                  bool equalColumnWidth = false,
                  const QString &rowPrefix = "");
QString prompt(
    const QString &display,
    const QStringList &allowedValues = {},
    int defaultValue = -1,
    QString verifier(const QString &value) = [](auto) { return QString(); });
QString error();
QString info();
QString note();
QString positive();
bool parseBool(const QString &str, bool &out);
QString strBool(bool value);
QString strGodotVersion(const GodotVersion *version);
QString strGodotVersion(const BoundGodotVersion *version);

} // namespace cli

#endif // INTERFACE_H
