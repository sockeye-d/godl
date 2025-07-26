#include "editcommand.h"
#include "cli/ansi.h"
#include "cli/interface.h"
#include "cli/versioncommand.h"
#include "godotproject.h"
#include "godotversion.h"
#include "util.h"

#include <QDir>

bool loadProject(GodotProject *&out)
{
    GodotProject *proj = GodotProject::load(QDir::currentPath() / "project.godot");
    if (!proj) {
        qStdOut() << cli::error() << "no project found in directory" << cli::ansi::nl;
        qStdOut().flush();
        return true;
    }
    out = proj;
    return false;
}

namespace cli::edit {

int edit(const Parser &)
{
    GodotProject *proj;
    if (loadProject(proj)) {
        return 1;
    }

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

    delete proj;
    return 0;
}

int configure(const Parser &parser)
{
    GodotProject *proj;
    if (loadProject(proj)) {
        return 1;
    }
    auto projDeleter = qScopeGuard([proj]() { delete proj; });
    if (parser.set("name")) {
        const QString &newValue = parser.op("name").param("value");
        if (newValue.isEmpty()) {
            qStdOut() << proj->name() << ansi::nl;
        } else {
            proj->setName(newValue);
        }
    }
    if (parser.set("description")) {
        const QString &newValue = parser.op("description").param("value");
        if (newValue.isEmpty()) {
            qStdOut() << proj->description() << ansi::nl;
        } else {
            proj->setDescription(newValue);
        }
    }
    if (parser.set("icon")) {
        QString newValue = parser.op("icon").param("value");
        if (newValue.isEmpty()) {
            qStdOut() << proj->icon() << ansi::nl;
        } else {
            QFileInfo file(QDir::current(), newValue);
            if (!file.exists()) {
                return 1;
            }
            proj->setIcon(newValue);
        }
    }
    return 0;
}

int tags::remove(const Parser &parser)
{
    GodotProject *proj;
    if (loadProject(proj)) {
        return 1;
    }
    const QStringList &tags = parser.op("remove").param("tag").split(",");

    for (const QString &tag : tags)
        if (!proj->tags().contains(tag)) {
            qStdOut() << error() << "no tag " << tag << " found." << ansi::nl;
            qStdOut().flush();
            return 1;
        }

    QStringList newTags = proj->tags();
    for (const QString &tag : tags)
        newTags.removeOne(tag);
    proj->setTags(newTags);

    delete proj;
    return 0;
}

int tags::add(const Parser &parser)
{
    GodotProject *proj;
    if (loadProject(proj)) {
        return 1;
    }
    const QStringList &tags = parser.op("add").param("tag").split(",");
    for (const QString &tag : tags) {
        if (proj->tags().contains(tag)) {
            qStdOut() << error() << "project already has tag " << tag << ansi::nl;
            qStdOut().flush();
            return 1;
        }
    }

    proj->setTags(proj->tags() << tags);

    delete proj;
    return 0;
}

int tags::list(const Parser &)
{
    GodotProject *proj;
    if (loadProject(proj)) {
        return 1;
    }

    qStdOut() << proj->tags().join(ansi::nl);

    if (proj->tags().isEmpty()) {
        qStdOut() << note() << "no tags have been configured for this project" << ansi::nl;
    }

    delete proj;
    return 0;
}

int bind(const Parser &parser)
{
    const QString &repo = parser.op("repo").param("repo");
    const QString &tag = parser.op("tag").param("tag");
    const QStringList &assetFilters = parser.op("run").param("filter-term").split(",");
    GodotProject *proj;
    if (loadProject(proj)) {
        return 1;
    }

    GodotVersion *version;
    if (getVersion(version, repo, tag, assetFilters)) {
        return 1;
    }

    proj->setGodotVersion(version->boundVersion());

    delete proj;
    delete version;
    return 0;
}

} // namespace cli::edit
