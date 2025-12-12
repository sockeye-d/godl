#include "cli/run.h"
#include "cli/versioncommand.h"
#include "dateconverter.h"
#include "godlapp.h"
#include "networkresponsecode.h"
#include "sizeconverter.h"
#include "util/iconconverter.h"
#include "versioninfo.h"
#include "widgets/betterfiledialog.h"
#include <KAboutData>
#include <KIconTheme>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KSharedConfig>
#include <QApplication>
#include <QMainWindow>
#include <QQuickStyle>
#include <QtQml>
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
#include "downloadmanager.h"
#include "main.h"
#include "projectsregistry.h"
#include "projecttemplates.h"
#include "versionregistry.h"
#include <KConfigDialog>
#include <KIconColors>

using namespace Qt::Literals::StringLiterals;

#define registerType(uri, major, minor, type) qmlRegisterType<type>(uri, major, minor, #type);

#define registerSingletonPtr(uri, major, minor, type) \
    registerSingletonPtrSpecialNamed(uri, major, minor, type, #type, type::instance())

#define registerSingletonPtrSpecialNamed(uri, major, minor, type, name, method) \
    qmlRegisterSingletonType<type>(uri, major, minor, name, [](QQmlEngine *, QJSEngine *) { \
        return method; \
    })

int main(int argc, char *argv[])
{
    if (argc > 1) {
        return cli::run::runCli(argc, argv);
    }

    KIconTheme::initTheme();
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme("godl"));

    KLocalizedString::setApplicationDomain("godl");
    cli::run::setAppMetadata();
    QApplication::setDesktopFileName(QStringLiteral("io.github.sockeye_d.godl"));
    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/");

    QApplication::setStyle(QStringLiteral("Breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    ProjectTemplates::instance()->extractDefault();

    KAboutData aboutData(QStringLiteral("godl"),
                         i18nc("@title", "godl"),
                         VersionInfo::instance()->tag(),
                         i18n("Godot version manager and downloader"),
                         KAboutLicense::MIT,
                         i18n("(c) 2025"));

    aboutData.setHomepage(QStringLiteral("https://github.com/sockeye-d/godl"));
    aboutData.setBugAddress("https://github.com/sockeye-d/godl/issues/new"_ba);
    aboutData.setDesktopFileName("dev.fishies.godl");

    KAboutData::setApplicationData(aboutData);
    registerType("dev.fishies.godl", 0, 1, ChainedJsonRequest);
    registerType("dev.fishies.godl", 0, 1, DownloadManager);
    registerType("dev.fishies.godl", 0, 1, DownloadManagerModel);
    registerType("dev.fishies.godl", 0, 1, DownloadInfo);
    registerType("dev.fishies.godl", 0, 1, GodlApp);
    registerType("dev.fishies.godl", 0, 1, BetterFileDialog);
    registerType("dev.fishies.godl", 0, 1, GodotVersion);
    registerType("dev.fishies.godl", 0, 1, GodotProject);
    registerType("dev.fishies.godl", 0, 1, ProjectsRegistryModel);
    registerType("dev.fishies.godl.qwidgets", 0, 1, QFileDialog);
    registerType("dev.fishies.godl.qwidgets", 0, 1, QDir);
    registerType("dev.fishies.godl.qwidgets", 0, 1, QAction);
    registerType("dev.fishies.godl.qwidgets", 0, 1, QIcon);
    qmlRegisterType<Template>("dev.fishies.godl", 0, 1, "projectTemplate");

    registerSingletonPtr("dev.fishies.godl", 0, 1, ConfigSignals);
    registerSingletonPtr("dev.fishies.godl", 0, 1, DateConverter);
    registerSingletonPtr("dev.fishies.godl", 0, 1, IconConverter);
    registerSingletonPtr("dev.fishies.godl", 0, 1, SizeConverter);
    registerSingletonPtr("dev.fishies.godl", 0, 1, NetworkResponseCode);
    registerSingletonPtr("dev.fishies.godl", 0, 1, ProjectsRegistry);
    registerSingletonPtr("dev.fishies.godl", 0, 1, ProjectTemplates);
    registerSingletonPtr("dev.fishies.godl", 0, 1, VersionInfo);
    registerSingletonPtr("dev.fishies.godl", 0, 1, VersionRegistry);
    registerSingletonPtrSpecialNamed("dev.fishies.godl",
                                     0,
                                     1,
                                     Config,
                                     "Configuration",
                                     Config::self());
    QQmlApplicationEngine engine;
    Main::engine = &engine;

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.load(QUrl("qrc:/dev/fishies/godl/ui/Main.qml"));

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
