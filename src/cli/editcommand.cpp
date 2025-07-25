#include "editcommand.h"
#include "cli/ansi.h"
#include "cli/interface.h"
#include "godotproject.h"
#include "util.h"

#include <QDir>

namespace cli::edit {

int edit(const Parser &)
{
    GodotProject *proj = GodotProject::load(QDir::currentPath() / "project.godot");
    auto openError = proj->openQuiet();
    switch (openError) {
    case GodotProject::NoError:
        qStdOut() << positive() << "opened project " << proj->path();
        break;
    case GodotProject::NoEditorBound:
        qStdOut() << error() << "no editor bound, bind one using 'edit bind'";
        break;
    case GodotProject::NoEditorFound:
        qStdOut() << error() << "no editor found, install " << proj->godotVersion()->toString();
        break;
    case GodotProject::FailedToStartEditor:
        qStdOut() << error() << "failed to start editor";
        break;
    }

    return 0;
}

} // namespace cli::edit
