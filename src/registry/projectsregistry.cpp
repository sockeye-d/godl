#include "projectsregistry.h"
#include <QDir>
#include "util.h"

void ProjectsRegistry::scan(const QString directory)
{
    const auto files = QDir(directory).entryList(QDir::Files);
    for (const QString &file : files) {
        if (file == "project.godot") {
            import(directory / file);
        }
    }

    const auto dirs = QDir(directory).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &dir : dirs) {
        scan(directory / dir);
    }
}

void ProjectsRegistry::import(const QString filepath)
{
    auto project = GodotProject::load(filepath);
    model()->append(std::move(project));
}
