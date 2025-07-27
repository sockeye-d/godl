#include "importcommand.h"
#include "cli/ansi.h"
#include "cli/interface.h"
#include "cliutil.h"
#include "projectsregistry.h"

#include <QDir>
#include <qthread.h>

using namespace std::chrono_literals;

namespace cli {

int import(const Parser &parser)
{
    QString defaultPath = QDir::currentPath();
    const QString &path = makeAbsolute(parser.op("import").param("path", defaultPath));
    bool scan = parser.set("recursive");

    // qStdOut() << path;

    if (scan) {
        QStringList result;
        ProjectsRegistry::instance()->scan(path, &result);
        qStdOut() << ansi::cursorHide;
        qStdOut().flush();

        std::chrono::nanoseconds nowTime = std::chrono::system_clock::now().time_since_epoch();
        bool scanned = false;
        while (ProjectsRegistry::instance()->scanning()) {
            QCoreApplication::processEvents();
            qStdOut() << ansi::cr << ansi::eraseInLine << progressBar(nowTime, "Scanning");
            qStdOut().flush();
            QThread::sleep(10ms);
            scanned = true;
        }

        qStdOut() << ansi::cursorShow;
        if (scanned) {
            qStdOut() << ansi::nl;
        }
        qStdOut() << positive() << "finished scanning" << ansi::nl;
        qStdOut() << note() << "found " << QString::number(result.size())
                  << " projects:" << ansi::nl << result.join("\n") << ansi::nl;

        qStdOut().flush();
    } else {
        const auto loadError = ProjectsRegistry::instance()->import(
            QFileInfo(path).isDir() ? path / "project.godot" : path);

        switch (loadError) {
        case ProjectsRegistry::LoadedSucessfully:
            qStdOut() << positive() << "imported project at " << path;
            break;
        case ProjectsRegistry::AlreadyLoaded:
            qStdOut() << error() << "already imported project at " << path;
            return 1;
        case ProjectsRegistry::CantLoad:
            qStdOut() << error() << "can't import project at " << path
                      << " (maybe it isn't a project directory?)";
            return 1;
        }
    }

    return 0;
}

} // namespace cli
