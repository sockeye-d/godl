#include "testcommand.h"

#include "cli/ansi.h"
#include "cli/interface.h"

#include <QCoreApplication>
#include <QThread>

using namespace Qt::StringLiterals;
using namespace std::chrono_literals;

namespace cli::test {

int testBars(const Parser &parser)
{
    bool ok;
    int ticks = 200;
    if (!parser.params("ticks").isEmpty()) {
        ticks = parser.params("ticks").constFirst().toInt(&ok);
        if (!ok || ticks < 0) {
            qStdOut() << "Couldn't parse ticks";
            qStdOut().flush();
            return 1;
        }
    }
    std::chrono::nanoseconds nowTime = std::chrono::system_clock::now().time_since_epoch();
    qStdOut() << cli::ansi::cursorHide;
    if (parser.set("indeterminate")) {
        for (int i = 0; i < ticks; i++) {
            QCoreApplication::processEvents();
            qStdOut() << cli::ansi::cr << cli::ansi::eraseInLine
                      << cli::progressBar(nowTime, "Indeterminate progress bar");
            qStdOut().flush();
            QThread::sleep(10ms);
        }
        qStdOut() << ansi::nl;
        for (int i = 0; i < ticks; i++) {
            QCoreApplication::processEvents();
            qStdOut() << cli::ansi::cr << cli::ansi::eraseInLine
                      << cli::progressBar(nowTime,
                                          "Indeterminate progress bar with a really long text");
            qStdOut().flush();
            QThread::sleep(10ms);
        }
        qStdOut() << ansi::nl;
    }

    if (parser.set("determinate")) {
        for (double p = 0.0; p < 1.0; p += 1.0 / ticks) {
            QCoreApplication::processEvents();
            qStdOut() << ansi::cr << ansi::eraseInLine
                      << cli::progressBar(nowTime, "Determinate progress bar", p);
            qStdOut().flush();
            QThread::sleep(10ms);
        }
        qStdOut() << ansi::nl;
    }
    qStdOut() << cli::ansi::cursorShow;
    qStdOut().flush();
    return 0;
}

int testUnderline(const Parser &)
{
    const static QString testMessage = "0123456789";
    qStdOut() << ansi::underline(testMessage) << ansi::nl;
    qStdOut() << ansi::underline(testMessage, 5, 8) << ansi::nl;
    qStdOut() << ansi::underline(testMessage, 5) << ansi::nl;
    return 0;
}

} // namespace cli::test
