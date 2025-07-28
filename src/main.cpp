#include <QApplication>
#include <QMainWindow>
#include <QQuickStyle>
#include <QtQml>
#include "cli/ansi.h"
#include "cli/createcommand.h"
#include "cli/editcommand.h"
#include "cli/gconfigcommand.h"
#include "cli/importcommand.h"
#include "cli/interface.h"
#include "cli/remotecommand.h"
#include "cli/testcommand.h"
#include "cli/versioncommand.h"
#include "dateconverter.h"
#include "godlapp.h"
#include "networkresponsecode.h"
#include "util/iconconverter.h"
#include "widgets/betterfiledialog.h"
#include <KAboutData>
#include <KIconTheme>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KSharedConfig>
#if __has_include("config.h")
#define CONFIG
#include "config.h"
#include "configsignals.h"
#endif
#include <QDesktopServices>
#include <QIconEngine>
#include <QMenuBar>
#include <QWindow>
#include <QtSystemDetection>
#include "chainedjsonrequest.h"
#include "cli/installcommand.h"
#include "cli/parser.h"
#include "downloadmanager.h"
#include "main.h"
#include "projectsregistry.h"
#include "projecttemplates.h"
#include "versionregistry.h"
#include <KConfigDialog>
#include <KIconColors>

using namespace Qt::Literals::StringLiterals;

#define registerSingletonPtr(uri, major, minor, type) \
    qmlRegisterSingletonType<type>("org.fishy.godl", \
                                   major, \
                                   minor, \
                                   #type, \
                                   [](QQmlEngine *, QJSEngine *) { return type::instance(); })

#define registerSingletonPtrSpecial(uri, major, minor, type, method) \
    qmlRegisterSingletonType<type>("org.fishy.godl", \
                                   major, \
                                   minor, \
                                   #type, \
                                   [](QQmlEngine *, QJSEngine *) { return method; })

#define registerSingletonPtrSpecialNamed(uri, major, minor, type, name, method) \
    qmlRegisterSingletonType<type>("org.fishy.godl", \
                                   major, \
                                   minor, \
                                   name, \
                                   [](QQmlEngine *, QJSEngine *) { return method; })

QString prependLines(QString string, QString prepension)
{
    static QRegularExpression re("^", QRegularExpression::MultilineOption);
    return string.replace(re, prepension);
}

QString printFs(const QString &path)
{
    QString current = "";
    QDirIterator dirs = QDirIterator(path, QDir::Dirs);
    QDirIterator files = QDirIterator(path, QDir::Files);
    while (dirs.hasNext()) {
        dirs.next();
        auto prefix = files.hasNext() || dirs.hasNext() ? u"│"_s : u" "_s;
        auto preprefix = files.hasNext() || dirs.hasNext() ? u"├╮"_s : u"╰"_s;
        if (dirs.filePath() == "") {
            continue;
        }
        current
            += prependLines(printFs(dirs.filePath()), prefix).prepend(" ").replace(0, 2, preprefix);
    }

    while (files.hasNext()) {
        files.next();
        if (files.fileName() == "") {
            continue;
        }
        auto preprefix = files.hasNext() ? u"├"_s : u"╰"_s;
        current += preprefix % files.fileName() % "\n";
    }

    return path % "\n" % current;
}

void setAppMetadata()
{
    QCoreApplication::setOrganizationName(QStringLiteral("fishy"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("fishy.org"));
    QCoreApplication::setApplicationName(QStringLiteral("godl"));
}

#define nonTerminalParse() \
    if (parser.parse(args)) { \
        return 1; \
    }
#define terminalParse() \
    if (parser.parse(args, false)) { \
        if (parser.set("help")) { \
            qStdOut() << parser.helpText(); \
            return 0; \
        } \
        return 1; \
    } \
    if (parser.set("help")) { \
        qStdOut() << parser.helpText(); \
        return 0; \
    }

void addListOptions(Parser &parser, const QString &label)
{
    using enum Parser::Option::Mode;
    parser.addOption(Parser::Option(Command, "add", {"add"}, "Add a " + label, {{label, ""}}));
    parser.addOption(
        Parser::Option(Command, "remove", {"remove", "rm"}, "Remove a " + label, {{label, ""}}));
}

int runCli(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    setAppMetadata();
    QStringList args;
    for (int i = 1; i < argc; i++) {
        args << QString::fromLocal8Bit(argv[i]);
    }
    Parser parser;
    auto errorPrinter = qScopeGuard([&parser]() {
        if (!parser.errors().empty()) {
            for (const auto &errors = parser.errors(); const QString &error : errors) {
                qStdOut() << cli::error() << error << cli::ansi::nl;
            }
        }
    });
    using enum Parser::Option::Mode;
    parser.addOption(Parser::Option(Command,
                                    "install",
                                    {"install"},
                                    "Install a Godot version",
                                    {{{"repo"}, {"The repo to install from"}},
                                     {{"tag"}, {"The tag name to get it from"}},
                                     {{"asset"}, {"The asset to install"}}}));
    parser.addOption(Parser::Option(Command, "edit", {"edit", "e"}, "Edit a Godot project"));
    parser.addOption(
        Parser::Option(Command, "version", {"version", "ver"}, "Modify downloaded versions"));
    parser.addOption(
        Parser::Option(Command, "godl-config", {"g-config", "g-cfg"}, "Modify godl configuration"));
    parser.addOption(Parser::Option(Switch,
                                    "help",
                                    {"help", "h", "?"},
                                    "Show this help message. Can also be combined with a "
                                    "command to show specific help about that command"));
    parser.addOption(Parser::Option(Switch, "verbose", {"verbose", "v"}, "Enable debug output"));
    parser.addOption(
        Parser::Option(Command, "remote", {"remote"}, "Lookup information on remote versions"));
    parser.addOption(Parser::Option(
        Command,
        "import",
        {"import", "im"},
        "Import a project",
        {{"path",
          "The path to the project. If not given, the current directory will be used",
          true}}));
    parser.addOption(Parser::Option(Command,
                                    "create",
                                    {"create", "new"},
                                    "Create a new project according to a template",
                                    {{"template", "The template to use", true}}));
#ifdef DEBUG
    parser.addOption(Parser::Option(Command, "test", {"test"}, "Test CLI tool"));
#endif
    nonTerminalParse();
    if (parser.set("verbose")) {
        QLoggingCategory::setFilterRules("");
    } else {
        QLoggingCategory::setFilterRules("*=false\n");
    }
    if (parser.set("create")) {
        parser.clearCommands({"create"});
        ProjectTemplates::instance()->extractDefault();
        ProjectTemplates::instance()->rescan();

        if (!parser.op("create").hasParam("template")) {
            return cli::list(parser);
        }

        parser.addOption(Parser::Option(Switch,
                                        "path",
                                        {"path", "p"},
                                        "The path to the new project. Defaults to the current path "
                                        "joined with the project name",
                                        {{"path"}}));
        parser.addOption(
            Parser::Option(Switch, "name", {"name", "n"}, "The project name", {{"name"}}));
        parser.addOption(
            Parser::Option(Switch,
                           "force",
                           {"force", "f"},
                           "Force create the project even if the path already exists"));
        parser.addOption(Parser::Option(Switch,
                                        "skip-menu",
                                        {"skip-menu", "s"},
                                        "Skip showing project configuration menu"));

        if (cli::addTemplateParams(parser)) {
            return 1;
        }

        terminalParse();
        return cli::create(parser);
    }
    if (parser.set("import")) {
        parser.clearCommands({"import"});
        parser.addOption(Parser::Option(Switch,
                                        "recursive",
                                        {"recursive", "r"},
                                        "Import projects recursively (e.g. scan)"));
        parser.addOption(Parser::Option(Switch,
                                        "dry-run",
                                        {"dry-run", "d"},
                                        "Don't import any projects, only print the results"));
        terminalParse();
        return cli::import(parser);
    }
    if (parser.set("godl-config")) {
        parser.clearCommands({"godl-config"});
        parser.addOption(Parser::Option(Command,
                                        "godot-location",
                                        {"godot-location"},
                                        "Where the Godot versions get downloaded. Defaults to "
                                            + Config::defaultGodotLocationValue(),
                                        {{"value", "", true}}));
        parser.addOption(
            Parser::Option(Command,
                           "project-location",
                           {"project-location"},
                           "The default location to search in for projects. Defaults to"
                               + Config::defaultProjectLocationValue(),
                           {{"value", "", true}}));
        parser.addOption(Parser::Option(Command,
                                        "template-location",
                                        {"template-location"},
                                        "Where the templates are located. Defaults to "
                                            + Config::defaultTemplateLocationValue(),
                                        {{"value", "", true}}));
        parser.addOption(
            Parser::Option(Command,
                           "default-location",
                           {"default-location"},
                           "The default template to use when creating a new project. Defaults to "
                               + Config::defaultTemplateLocationValue(),
                           {{"value", "", true}}));
        parser.addOption(
            Parser::Option(Command, "download", {"download", "dl"}, "Modify download settings"));
        parser.addOption(
            Parser::Option(Command, "version", {"version", "ver"}, "Modify version settings"));
        nonTerminalParse();
        if (parser.set("download")) {
            parser.clearCommands({"godl-config", "download"});
            parser.addOption(
                Parser::Option(Command, "filter", {"filter"}, "Modify download filters"));
            parser.addOption(Parser::Option(Command, "source", {"source"}, "Modify sources"));
            nonTerminalParse();
            if (parser.set("filter")) {
                parser.clearCommands({"godl-config", "download", "filter"});
                addListOptions(parser, "filter");
                terminalParse();
                if (parser.set("add")) {
                    return cli::filter::add(parser);
                }
                if (parser.set("remove")) {
                    return cli::filter::remove(parser);
                }
                return cli::filter::list(parser);
            }
            if (parser.set("source")) {
                parser.clearCommands({"godl-config", "download", "source"});
                addListOptions(parser, "source");
                parser.addOption(Parser::Option(
                    Command,
                    "set-default",
                    {"default"},
                    "Get or set the default source for commands like 'remote list <tag>'",
                    {{"default", "", true}}));
                terminalParse();
                if (parser.set("add")) {
                    return cli::source::add(parser);
                }
                if (parser.set("remove")) {
                    return cli::source::remove(parser);
                }
                if (parser.set("set-default")) {
                    return cli::source::setDefault(parser);
                }
                return cli::source::list(parser);
            }

            terminalParse();
            return 0;
        }
        if (parser.set("version")) {
            parser.clearCommands({"godl-config", "download", "version"});
            parser.addOption(
                Parser::Option(Command,
                               "default-command",
                               {"command", "cmd"},
                               "The default command to use for new versions. Defaults to "
                                   + Config::defaultCommand(),
                               {{"value", "", true}}));
            parser.addOption(Parser::Option(
                Command,
                "cache",
                {"cache"},
                "Don't delete deleted versions from the temp directory after they're downloaded. "
                "You probably want to keep this off - it's mostly for development. Defaults to"
                    + Config::defaultCommand(),
                {{"value", "", true}}));
            return cli::versionConfig(parser);
        }

        terminalParse();
        // TODO: this
        return cli::general(parser);
    }
    if (parser.set("install")) {
        parser.clearCommands({"install"});
        parser.addOption(Parser::Option(Switch,
                                        "force",
                                        {"force", "f"},
                                        "Force installation, overriding if the particular version "
                                        "had already been downloaded"));
        terminalParse();
        return cli::install::install(parser);
    }
    if (parser.set("version")) {
        parser.clearCommands({"version"});
        parser.addOption(Parser::Option(Command,
                                        "remove",
                                        {"remove", "rm"},
                                        "Remove a downloaded version",
                                        {{{"filter-term"}, {"The filter terms to use"}}}));
        parser.addOption(
            Parser::Option(Command, "list", {"list", "ls", "l"}, "List downloaded versions"));
        parser.addOption(Parser::Option(Command,
                                        "run",
                                        {"run"},
                                        "Run a downloaded version",
                                        {{"filter-term", "The filter terms to use"}}));
        parser.addOption(
            Parser::Option(Command,
                           "command",
                           {"command", "cmd"},
                           "Set or get the command for a version",
                           {{"filter-term", "The filter terms to use"}, {"cmd", "", true}}));
        nonTerminalParse();
        if (parser.set("list")) {
            parser.clearCommands({"version", "list"});
            terminalParse();
            return cli::version::list(parser);
        }
        if (parser.set("remove")) {
            parser.clearCommands({"version", "remove"});
            parser.addOption(Parser::Option(Switch,
                                            "repo",
                                            {"repo", "r"},
                                            "The repository to use",
                                            {{"repo", ""}}));
            parser.addOption(
                Parser::Option(Switch, "tag", {"tag", "t"}, "The tag to use", {{"tag", ""}}));
            parser.addOption(Parser::Option(Switch,
                                            "force",
                                            {"force", "f"},
                                            "Force remove without confirmation"));
            terminalParse();
            return cli::version::remove(parser);
        }
        if (parser.set("run")) {
            parser.clearCommands({"version", "run"});
            parser.addOption(Parser::Option(Switch,
                                            "repo",
                                            {"repo", "r"},
                                            "The repository to use",
                                            {{"repo", ""}}));
            parser.addOption(
                Parser::Option(Switch, "tag", {"tag", "t"}, "The tag to use", {{"tag", ""}}));
            parser.addOption(
                Parser::Option(Switch,
                               "output",
                               {"output", "o"},
                               "Show the output of the Godot executable. Not recommended, because "
                               "the godl process exits before Godot does"));
            terminalParse();
            return cli::version::run(parser);
        }
        if (parser.set("command")) {
            parser.clearCommands({"version", "command"});
            parser.addOption(Parser::Option(Switch,
                                            "repo",
                                            {"repo", "r"},
                                            "The repository to use",
                                            {{"repo", ""}}));
            parser.addOption(
                Parser::Option(Switch, "tag", {"tag", "t"}, "The tag to use", {{"tag", ""}}));
            terminalParse();
            return cli::version::command(parser);
        }

        terminalParse();
        return 0;
    }
    if (parser.set("edit")) {
        parser.clearCommands({"edit"});
        parser.addOption(Parser::Option(Switch,
                                        "path",
                                        {"path", "p"},
                                        "Path to the Godot project",
                                        {{"path", ""}}));
        parser.addOption(Parser::Option(Command,
                                        "bind",
                                        {"bind"},
                                        "Bind an editor to a project",
                                        {{"filter-term", "The term to filter editors by", true}}));
        parser.addOption(Parser::Option(Command,
                                        "configure",
                                        {"configure", "cfg"},
                                        "Configure project name, description, icon, and tags"));
        nonTerminalParse();
        if (parser.set("bind")) {
            parser.clearCommands({"edit", "bind"});
            parser.addOption(Parser::Option(Switch,
                                            "repo",
                                            {"repo", "r"},
                                            "The repository to use",
                                            {{"repo", "", false}}));
            parser.addOption(
                Parser::Option(Switch, "tag", {"tag", "t"}, "The tag to use", {{"tag", ""}}));
            parser.addOption(Parser::Option(Switch, "unbind", {"unbind", "u"}, "Unbind the editor"));
            terminalParse();
            return cli::edit::bind(parser);
        }
        if (parser.set("configure")) {
            parser.clearCommands({"edit", "configure"});
            parser.addOption(
                Parser::Option(Switch,
                               "name",
                               {"name", "n"},
                               "Set the name of the project",
                               {{"value", "If not given, it'll output the current value", true}}));
            parser.addOption(
                Parser::Option(Switch,
                               "description",
                               {"description", "d"},
                               "Set the description of the project",
                               {{"value", "If not given, it'll output the current value", true}}));
            parser.addOption(
                Parser::Option(Switch,
                               "icon",
                               {"icon", "i"},
                               "Modify the icon of the project",
                               {{"value",
                                 "The path to the icon, relative to the root directory of the "
                                 "project. If not given, it'll output the current value",
                                 true}}));
            parser.addOption(
                Parser::Option(Switch,
                               "favorite",
                               {"favorite", "f"},
                               "Favorite or unfavorite the project. This value is stored in the "
                               "local configuration, not in the project file",
                               {{"value", "", true}}));
            parser.addOption(Parser::Option(
                Command,
                "tag",
                {"tag", "t"},
                "Modify project tags. If no subcommand is used, it'll list the tags"));
            nonTerminalParse();
            if (parser.set("tag")) {
                parser.clearCommands({"edit", "configure", "tag"});
                addListOptions(parser, "tag");
                terminalParse();
                if (parser.set("add")) {
                    return cli::edit::tags::add(parser);
                }
                if (parser.set("remove")) {
                    return cli::edit::tags::remove(parser);
                }
                return cli::edit::tags::list(parser);
            }

            terminalParse();
            return cli::edit::configure(parser);
        }

        parser.addOption(
            Parser::Option(Switch,
                           "extra-args",
                           {"args", "a"},
                           "Extra arguments to be passed to Godot",
                           {{"args",
                             "A space-separated string of arguments. {projectPath} gets replaced "
                             "by the actual project path"}}));
        parser.addOption(
            Parser::Option(Switch,
                           "no-default-args",
                           {"no-args", "n"},
                           "Don't pass the executable-specified arguments (e.g. --editor)"));
        terminalParse();
        return cli::edit::edit(parser);
    }
    if (parser.set("remote")) {
        parser.clearCommands({"remote"});
        parser.addOption(Parser::Option(
            Switch,
            "repo",
            {"repo", "r"},
            "The repository to look in. If not specified, the default repository is used.",
            {{"repo", ""}}));
        parser.addOption(Parser::Option(Command,
                                        "list",
                                        {"list", "ls"},
                                        "List the assets for a given tag",
                                        {{"tag",
                                          "The tag to list the assets for. If set to 'tag', it'll "
                                          "output the tags on the server instead"}}));
        nonTerminalParse();
        if (parser.set("list")) {
            parser.clearCommands({"remote", "list"});
            if (parser.op("list").param("tag") == "tag") {
                parser.addOption(
                    Parser::Option(Switch,
                                   "list-all",
                                   {"all", "a"},
                                   "List all the tags, not just the first 25. Will take longer "
                                   "since it needs to make multiple requests"));
                terminalParse();
                return cli::remote::listTags(parser);
            } else {
                parser.addOption(Parser::Option(
                    Switch,
                    "list-all",
                    {"all", "a"},
                    "List all the releases, not just the recommended ones for your system"));
                terminalParse();
                return cli::remote::list(parser);
            }
            terminalParse();
            return 0;
        }

        terminalParse();
        return 0;
    }
    if (parser.set("test")) {
        parser.clearCommands({"test"});
        parser.addOption(Parser::Option(Command, "bar", {"bar", "b"}, "Test progress bars"));
        parser.addOption(
            Parser::Option(Command, "underline", {"underline", "u"}, "Test underlines"));
        nonTerminalParse();

        if (parser.set("bar")) {
            parser.clearCommands({"test", "bar"});
            parser.addOption(
                Parser::Option(Switch, "determinate", {"d"}, "Test determinate progress bars"));
            parser.addOption(
                Parser::Option(Switch, "indeterminate", {"i"}, "Test indeterminate progress bars"));
            parser.addOption(Parser::Option(Switch,
                                            "ticks",
                                            {"ticks", "t"},
                                            "How many ticks (each tick is 10ms) to run for",
                                            {{"ticks", "the number of ticks"}}));
            terminalParse();
            return cli::test::testBars(parser);
        }
        if (parser.set("underline")) {
            terminalParse();
            return cli::test::testUnderline(parser);
        }
        terminalParse();
        return 0;
    }
    terminalParse();
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc > 1) {
        return runCli(argc, argv);
    }

    KIconTheme::initTheme();
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme("godl"));

    KLocalizedString::setApplicationDomain("godl");
    setAppMetadata();
    QApplication::setDesktopFileName(QStringLiteral("org.fishy.godl"));
    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/");

#ifdef Q_OS_LINUX
    QApplication::setStyle(QStringLiteral("Breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
#endif

#ifdef Q_OS_WIN32
    QApplication::setStyle(QStringLiteral("FluentWinUI3"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("FluentWinUI3"));
    }
#endif

#if 0
    QFile file(QStandardPaths::standardLocations(QStandardPaths::TempLocation).constFirst()
               / "qrcfs.txt");
    file.open(QFile::WriteOnly);
    file.write(printFs(":").toUtf8());
    file.close();
    QDesktopServices::openUrl(QUrl::fromLocalFile(file.fileName()));

    QFile f(":/templates/default/metatemplate.json");
    f.open(QFile::ReadOnly);
    debug() << f.readAll();

    return 0;
#endif

    ProjectTemplates::instance()->extractDefault();

    KAboutData aboutData(QStringLiteral("godl"),
                         i18nc("@title", "godl"),
                         QStringLiteral("0.1"),
                         i18n("Godot version manager and downloader"),
                         KAboutLicense::MIT,
                         i18n("(c) 2025"));

    aboutData.setHomepage(QStringLiteral("https://github.com/sockeye-d/godl"));
    aboutData.setBugAddress("https://github.com/sockeye-d/godl/issues/new"_ba);
    aboutData.setDesktopFileName("org.fishy.godl");

    KAboutData::setApplicationData(aboutData);
    qmlRegisterType<ChainedJsonRequest>("org.fishy.godl", 0, 1, "ChainedJsonRequest");
    qmlRegisterType<DownloadManager>("org.fishy.godl", 0, 1, "DownloadManager");
    qmlRegisterType<DownloadManagerModel>("org.fishy.godl", 0, 1, "DownloadManagerModel");
    qmlRegisterType<DownloadInfo>("org.fishy.godl", 0, 1, "DownloadInfo");
    qmlRegisterType<GodlApp>("org.fishy.godl", 0, 1, "GodlApp");
    qmlRegisterType<BetterFileDialog>("org.fishy.godl", 0, 1, "BetterFileDialog");
    qmlRegisterType<GodotVersion>("org.fishy.godl", 0, 1, "GodotVersion");
    qmlRegisterType<GodotProject>("org.fishy.godl", 0, 1, "GodotProject");
    qmlRegisterType<QFileDialog>("org.fishy.godl.qwidgets", 0, 1, "FileDialog");
    qmlRegisterType<QDir>("org.fishy.godl.qwidgets", 0, 1, "QDir");
    qmlRegisterType<QAction>("org.fishy.godl.qwidgets", 0, 1, "QAction");
    qmlRegisterType<QIcon>("org.fishy.godl.qwidgets", 0, 1, "QIcon");
    qmlRegisterType<ProjectsRegistryModel>("org.fishy.godl", 0, 1, "ProjectsRegistryModel");
    qmlRegisterType<Template>("org.fishy.godl",
                              0,
                              1,
                              "projectTemplate"); // clazy:exclude=lowercase-qml-type-name
    registerSingletonPtr("org.fishy.godl", 0, 1, IconConverter);
    registerSingletonPtr("org.fishy.godl", 0, 1, DateConverter);
    registerSingletonPtr("org.fishy.godl", 0, 1, ProjectsRegistry);
    registerSingletonPtr("org.fishy.godl", 0, 1, VersionRegistry);
    registerSingletonPtr("org.fishy.godl", 0, 1, NetworkResponseCode);
    registerSingletonPtrSpecialNamed("org.fishy.godl", 0, 1, Config, "Configuration", Config::self());
    registerSingletonPtr("org.fishy.godl", 0, 1, ConfigSignals);
    registerSingletonPtr("org.fishy.godl", 0, 1, ProjectTemplates);
    QQmlApplicationEngine engine;
    Main::engine = &engine;

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl("qrc:/org/fishy/godl/ui/Main.qml"));

    if (engine.rootObjects().isEmpty()) {
        Main::engine = nullptr;
        return -1;
    }

    auto return_code = app.exec();

    if (return_code == 0)
        Config::self()->save();

    Main::engine = nullptr;
    return return_code;
}
