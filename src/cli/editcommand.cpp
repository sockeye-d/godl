#include "editcommand.h"
#include "cli/ansi.h"
#include "cli/interface.h"
#include "cli/versioncommand.h"
#include "godotproject.h"
#include "godotversion.h"
#include "projectsregistry.h"
#include "util.h"

#include <QDir>
#include <QProcess>

bool loadProject(GodotProject *&out)
{
    GodotProject *proj = ProjectsRegistry::instance()->loadCli(QDir::currentPath()
                                                               / "project.godot");
    if (!proj) {
        qStdOut() << cli::error() << "no project found in directory" << cli::ansi::nl;
        qStdOut().flush();
        return true;
    }
    out = proj;
    return false;
}

namespace cli::edit {

int edit(const Parser &parser)
{
    GodotProject *proj;
    if (loadProject(proj)) {
        return 1;
    }

    if (parser.set("dry-run")) {
        auto cmd = proj->getResolvedCmd(parser.op("extra-args").param("args"),
                                        parser.set("no-default-args"));
        if (GodotProject::OpenError *openError = std::get_if<GodotProject::OpenError>(&cmd)) {
            switch (*openError) {
            case GodotProject::NoEditorBound:
                qStdOut() << error() << "no editor bound, bind one using 'edit bind'";
                break;
            case GodotProject::NoEditorFound:
                qStdOut() << error() << "no editor found, install "
                          << proj->godotVersion()->toString();
                break;
            case GodotProject::NoError:
            case GodotProject::FailedToStartEditor:
                break;
            }
            return 1;
        }
        qStdOut() << std::get<QString>(cmd) << flushnl();
        return 0;
    }

    auto openError = proj->openForCli(parser.op("extra-args").param("args"),
                                      parser.set("no-default-args"),
                                      parser.set("loud"));
    switch (openError.first) {
    case GodotProject::NoError:
        qStdOut() << positive() << "opened project " << proj->path() << flushnl();
        if (parser.set("loud")) {
            openError.second->waitForFinished(-1);
        }
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

    delete openError.second;
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
    if (parser.set("favorite")) {
        const QString &newValue = parser.op("favorite").param("value");
        if (newValue.isEmpty()) {
            qStdOut() << strBool(proj->favorite()) << ansi::nl;
        } else {
            bool favorite;
            if (parseBool(newValue, favorite)) {
                return 1;
            }
            proj->setFavorite(favorite);
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
    const QStringList &assetFilters = parser.op("bind").param("filter-term").split(",");
    GodotProject *proj;
    if (loadProject(proj)) {
        return 1;
    }

    if (parser.set("unbind")) {
        if (!proj->godotVersion()) {
            qStdOut() << error() << "project has no editor bound";
            return 1;
        }

        proj->setGodotVersion(nullptr);
        return 0;
    }

    if (!parser.op("bind").hasParam("filter-term")) {
        if (proj->godotVersion()) {
            qStdOut() << strGodotVersion(proj->godotVersion()) << ansi::nl;
        } else {
            qStdOut() << error() << "project has no editor bound" << ansi::nl;
        }
        return 0;
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
