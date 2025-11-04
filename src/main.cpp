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
    aboutData.setDesktopFileName("org.fishy.godl");

    KAboutData::setApplicationData(aboutData);
    registerType("org.fishy.godl", 0, 1, ChainedJsonRequest);
    registerType("org.fishy.godl", 0, 1, DownloadManager);
    registerType("org.fishy.godl", 0, 1, DownloadManagerModel);
    registerType("org.fishy.godl", 0, 1, DownloadInfo);
    registerType("org.fishy.godl", 0, 1, GodlApp);
    registerType("org.fishy.godl", 0, 1, BetterFileDialog);
    registerType("org.fishy.godl", 0, 1, GodotVersion);
    registerType("org.fishy.godl", 0, 1, GodotProject);
    registerType("org.fishy.godl", 0, 1, ProjectsRegistryModel);
    registerType("org.fishy.godl.qwidgets", 0, 1, QFileDialog);
    registerType("org.fishy.godl.qwidgets", 0, 1, QDir);
    registerType("org.fishy.godl.qwidgets", 0, 1, QAction);
    registerType("org.fishy.godl.qwidgets", 0, 1, QIcon);
    qmlRegisterType<Template>("org.fishy.godl", 0, 1, "projectTemplate");

    registerSingletonPtr("org.fishy.godl", 0, 1, ConfigSignals);
    registerSingletonPtr("org.fishy.godl", 0, 1, DateConverter);
    registerSingletonPtr("org.fishy.godl", 0, 1, IconConverter);
    registerSingletonPtr("org.fishy.godl", 0, 1, SizeConverter);
    registerSingletonPtr("org.fishy.godl", 0, 1, NetworkResponseCode);
    registerSingletonPtr("org.fishy.godl", 0, 1, ProjectsRegistry);
    registerSingletonPtr("org.fishy.godl", 0, 1, ProjectTemplates);
    registerSingletonPtr("org.fishy.godl", 0, 1, VersionInfo);
    registerSingletonPtr("org.fishy.godl", 0, 1, VersionRegistry);
    registerSingletonPtrSpecialNamed("org.fishy.godl", 0, 1, Config, "Configuration", Config::self());
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
