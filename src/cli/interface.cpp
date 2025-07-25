#include "interface.h"
#include "cli/ansi.h"

#include <QRegularExpression>

using namespace Qt::StringLiterals;
using namespace std::chrono;

int wrap(int x, int y)
{
    return (((x + y) % y) + y) % y;
}

namespace cli {

QString progressBar(nanoseconds startTime,
                    const QString &caption,
                    double progress,
                    const QString &rightCaption)
{
    QString bar;
    bar.reserve(marqueeWidth + separator.length() + barWidth);
    if (caption.length() <= marqueeWidth) {
        bar += u"%1"_s.arg(caption, -marqueeWidth);
    } else {
        int p = (int) (toSeconds(system_clock::now().time_since_epoch() - startTime) * 6.0)
                % (caption.length() + 1);
        QString realCaption = (caption + " ").repeated(2);
        realCaption.slice(p, marqueeWidth);
        bar += u"%1"_s.arg(realCaption, marqueeWidth);
    }
    if (!caption.isEmpty()) {
        bar += separator;
    }
    if (progress >= 0.0) {
        double barProgress = progress * barWidth;
        QString progressBar = "";
        progressBar += progressBarChars.constLast().repeated(int(barProgress));
        double rem;
        const double frac = modf(barProgress, &rem);
        progressBar += progressBarChars[int(frac * (progressBarChars.size() - 1))];
        // progressBar += "   ";
        // progressBar += reverseProgressBarChars[int(frac * (reverseProgressBarChars.size() - 1))];
        // progressBar += progressBarEmptyChar.repeated(int(barWidth - progress) - 4);
        // progressBar.slice(0, barWidth);
        bar += u"%1"_s.arg(progressBar, -barWidth);
    } else {
        double p = toSeconds(system_clock::now().time_since_epoch() - startTime);
#if 1
        for (int i = 0; i < barWidth; i++) {
            int ch = ((i % indeterminateChars.size()) * 8 + int(p * 32))
                     % indeterminateChars.size();
            bar += indeterminateChars[ch];
        }
#else
        bar += indeterminate[int(p * 16) % indeterminate.size()];
#endif
    }
    if (rightCaption.contains("{progress}") && progress < 0.0) {
        return bar;
    }

    QString realRightCaption = rightCaption;
    realRightCaption.replace("{progress}", QString::number(ceil(progress * 100), 'f', 0));
    if (!realRightCaption.isEmpty()) {
        bar += separator;
    }
    bar += realRightCaption;
    return bar;
}

QString asColumns(const QList<QStringList> &columns,
                  const QString &columnSeparator,
                  bool equalColumnWidth)
{
    const static QRegularExpression ansiRegex{"\e\[[0-9;]*m", QRegularExpression::MultilineOption};
    QList<int> columnWidths;
    for (int rowIndex = 0; rowIndex < columns.size(); rowIndex++) {
        const QStringList &row = columns[rowIndex];
        if (row.size() > columnWidths.size()) {
            columnWidths.resize(row.size());
        }

        for (int colIndex = 0; colIndex < row.size(); colIndex++) {
            QString col = row[colIndex];
            col.remove(ansiRegex);
            if (col.length() > columnWidths[colIndex]) {
                columnWidths[colIndex] = col.length() + columnSeparator.length();
            }
        }
    }
    if (equalColumnWidth) {
        int maxWidth = 0;
        for (int width : std::as_const(columnWidths)) {
            if (width > maxWidth) {
                maxWidth = width;
            }
        }
        columnWidths.assign(columnWidths.size(), maxWidth);
    }
    int totalWidth = 0;
    for (const int &width : std::as_const(columnWidths))
        totalWidth += width;
    QString result;
    result.reserve(totalWidth * columns.size());
    for (const QStringList &column : std::as_const(columns)) {
        for (int i = 0; i < column.size(); i++) {
            result += u"%1"_s.arg(column[i], -columnWidths[i] + 1);
            if (i != column.size() - 1)
                result += columnSeparator;
        }
        result += "\n";
    }
    return result;
}

QString prompt(const QString &display, const QStringList &allowedValues, int defaultValue)
{
    QStringList values = allowedValues;
    if (defaultValue >= 0) {
        values[defaultValue] = values[defaultValue].toUpper();
    }
    QString confirmation;
    do {
        qStdOut() << display << u" [%1] "_s.arg(values.join("/"));
        qStdOut().flush();
        confirmation = qStdIn().readLine();
        confirmation = confirmation.trimmed();
        if (defaultValue >= 0 && confirmation.isEmpty()) {
            confirmation = allowedValues[defaultValue];
        } else if (!allowedValues.contains(confirmation)) {
            qStdOut() << confirmation << " was not a valid input" << ansi::nl;
            confirmation = "";
        }
    } while (confirmation.isEmpty());
    return confirmation;
}

QString error()
{
    return ansi::fgcolor(ansi::Red) % u"error: "_s % ansi::reset;
}

QString info()
{
    return ansi::reset % u"info: "_s % ansi::reset;
}

QString note()
{
    return ansi::fgcolor(ansi::Cyan) % u"note: "_s % ansi::reset;
}

QString positive()
{
    return ansi::fgcolor(ansi::Green) % u"success: "_s % ansi::reset;
}

} // namespace cli

double toSeconds(std::chrono::nanoseconds time)
{
    return time.count() * 1e-9;
}
