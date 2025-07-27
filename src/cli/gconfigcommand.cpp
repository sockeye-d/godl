#include "gconfigcommand.h"
#include "cli/ansi.h"
#include "cli/interface.h"
#include "config.h"

QString makeAbsolute(const QString &path)
{
    QDir dir = QDir::current();
    dir.cd(path);
    return dir.absolutePath();
}

namespace cli {

int general(const Parser &parser)
{
    if (parser.set("godot-location")) {
        const QString &newValue = parser.op("godot-location").param("value");
        if (newValue.isEmpty()) {
            qStdOut() << Config::godotLocation();
            return 0;
        }

        Config::setGodotLocation(makeAbsolute(newValue));
        Config::self()->save();
        return 0;
    }
    if (parser.set("project-location")) {
        const QString &newValue = parser.op("project-location").param("value");
        if (newValue.isEmpty()) {
            qStdOut() << Config::projectLocation();
            return 0;
        }

        Config::setProjectLocation(makeAbsolute(newValue));
        Config::self()->save();
        return 0;
    }
    if (parser.set("template-location")) {
        const QString &newValue = parser.op("template-location").param("value");
        if (newValue.isEmpty()) {
            qStdOut() << Config::templateLocation();
            return 0;
        }

        Config::setTemplateLocation(makeAbsolute(newValue));
        Config::self()->save();
        return 0;
    }
    return 0;
}

int versionConfig(const Parser &parser)
{
    if (parser.set("default-command")) {
        const QString &newValue = parser.op("default-command").param("value");
        if (newValue.isEmpty()) {
            qStdOut() << Config::defaultCommand();
            return 0;
        }

        Config::setDefaultCommand(newValue);
        Config::self()->save();
        return 0;
    }
    if (parser.set("cache")) {
        const QString &newValue = parser.op("cache").param("value");
        if (newValue.isEmpty()) {
            qStdOut() << Config::cacheVersions();
            return 0;
        }

        bool value;
        if (parseBool(newValue, value)) {
            return 1;
        }

        Config::setCacheVersions(value);
        Config::self()->save();
        return 0;
    }
    return 0;
}

int filter::add(const Parser &parser)
{
    const QStringList &values = parser.op("add").param("filter").split(",");
    for (const QString &value : values) {
        if (Config::downloadFilter().contains(value)) {
            qStdOut() << error() << "already has filter " << value << ansi::nl;
            qStdOut().flush();
            return 1;
        }
    }

    Config::setDownloadFilter(Config::downloadFilter() << values);
    Config::self()->save();
    return 0;
}

int filter::remove(const Parser &parser)
{
    const QStringList &values = parser.op("remove").param("filter").split(",");

    for (const QString &value : values)
        if (!Config::downloadFilter().contains(value)) {
            qStdOut() << error() << "no filter " << value << " found." << ansi::nl;
            qStdOut().flush();
            return 1;
        }

    QStringList newValues = Config::downloadFilter();
    for (const QString &value : values)
        newValues.removeOne(value);
    Config::setDownloadFilter(newValues);
    Config::self()->save();

    return 0;
}

int filter::list(const Parser &)
{
    qStdOut() << Config::downloadFilter().join(ansi::nl);

    if (Config::downloadFilter().isEmpty()) {
        qStdOut() << note() << "no filters have been configured" << ansi::nl;
    }
    return 0;
}

int source::add(const Parser &parser)
{
    const QStringList &values = parser.op("add").param("source").split(",");
    for (const QString &value : values) {
        if (Config::sources().contains(value)) {
            qStdOut() << error() << "already has filter " << value << ansi::nl;
            qStdOut().flush();
            return 1;
        }
    }

    Config::setSources(Config::sources() << values);
    Config::self()->save();
    return 0;
}

int source::remove(const Parser &parser)
{
    const QStringList &values = parser.op("remove").param("source").split(",");

    for (const QString &value : values)
        if (!Config::sources().contains(value)) {
            qStdOut() << error() << "no source " << value << " found." << ansi::nl;
            qStdOut().flush();
            return 1;
        }

    QStringList newValues = Config::sources();
    for (const QString &value : values)
        newValues.removeOne(value);
    Config::setSources(newValues);
    Config::self()->save();

    return 0;
}

int source::list(const Parser &)
{
    qStdOut() << Config::sources().join(ansi::nl);

    if (Config::sources().isEmpty()) {
        qStdOut() << note() << "no sources have been configured" << ansi::nl;
    }
    return 0;
}

} // namespace cli
