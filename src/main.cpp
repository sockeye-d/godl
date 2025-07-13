#include <QApplication>
#include <QMainWindow>
#include <QQuickStyle>
#include <QtQml>
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
#include "chainedjsonrequest.h"
#include "downloadmanager.h"
#include "main.h"
#include "projectsregistry.h"
#include "versionregistry.h"
#include <KConfigDialog>

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

int main(int argc, char *argv[])
{
    KIconTheme::initTheme();
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("godl");
    QApplication::setOrganizationName(QStringLiteral("fishy"));
    QApplication::setOrganizationDomain(QStringLiteral("fishy.org"));
    QApplication::setApplicationName(QStringLiteral("godl"));
    QApplication::setDesktopFileName(QStringLiteral("org.fishy.godl"));

    QApplication::setStyle(QStringLiteral("breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    // KSharedConfigPtr config = KSharedConfig::openConfig(u"godl"_s);
    // auto config = Config::self();
    // debug() << config->godotLocation();

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

    // qmlRegisterSingletonType("org.fishy.godl",
    //                          0,
    //                          1,
    //                          "About",
    //                          [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
    //                              return engine->toScriptValue(KAboutData::applicationData());
    //                          });
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
    registerSingletonPtr("org.fishy.godl", 0, 1, IconConverter);
    registerSingletonPtr("org.fishy.godl", 0, 1, DateConverter);
    registerSingletonPtr("org.fishy.godl", 0, 1, ProjectsRegistry);
    registerSingletonPtr("org.fishy.godl", 0, 1, VersionRegistry);
    registerSingletonPtr("org.fishy.godl", 0, 1, NetworkResponseCode);
    registerSingletonPtrSpecial("org.fishy.godl", 0, 1, Config, Config::self());
    registerSingletonPtr("org.fishy.godl", 0, 1, ConfigSignals);
    QQmlApplicationEngine engine;
    Main::engine = &engine;

    Main::engine->rootContext()->setContextObject(new KLocalizedContext(&engine));
    Main::engine->loadFromModule(u"org.fishy.godl"_s, u"Main"_s);

    if (engine.rootObjects().isEmpty()) {
        Main::engine = nullptr;
        return -1;
    }

    auto return_code = app.exec();

    Config::self()->save();

    Main::engine = nullptr;
    return return_code;
}
