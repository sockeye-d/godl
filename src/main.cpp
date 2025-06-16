#include <QApplication>
#include <QMainWindow>
#include <QQuickStyle>
#include <QtQml>
#include <KAboutData>
#include <KIconTheme>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KSharedConfig>
#include <networkresponsecode.h>
#if __has_include("config.h")
#define CONFIG
#include "config.h"
#endif
#include "main.h"
#include <KConfigDialog>
#include <chainedjsonrequest.h>
#include <downloadmanager.h>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    KIconTheme::initTheme();
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("godl");
    QApplication::setOrganizationName(QStringLiteral("fishy"));
    QApplication::setOrganizationDomain(QStringLiteral("fishy.org"));
    QApplication::setApplicationName(QStringLiteral("godl"));
    QApplication::setDesktopFileName(QStringLiteral("org.fishy.godl"));

    // QApplication::setStyle(QStringLiteral("breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    // KSharedConfigPtr config = KSharedConfig::openConfig(u"godl"_s);
    // auto config = Config::self();
    // qDebug() << config->godotLocation();

    KAboutData aboutData(QStringLiteral("godl"),
                         i18nc("@title", "godl"),
                         QStringLiteral("0.1"),
                         i18n("Godot version manager and downloader"),
                         KAboutLicense::MIT,
                         i18n("(c) 2025"));

    aboutData.setHomepage(QStringLiteral("https://github.com/sockeye-d/godl"));
    aboutData.setBugAddress("https://github.com/sockeye-d/godl/issues/new"_ba);

    KAboutData::setApplicationData(aboutData);

    qmlRegisterSingletonType("org.fishy.godl",
                             0,
                             1,
                             "About",
                             [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
                                 return engine->toScriptValue(KAboutData::applicationData());
                             });
    qmlRegisterType<ChainedJsonRequest>("org.fishy.godl", 0, 1, "ChainedJsonRequest");
    qmlRegisterType<DownloadManager>("org.fishy.godl", 0, 1, "DownloadManager");
#ifdef CONFIG
    qmlRegisterSingletonInstance("org.fishy.godl", 0, 1, "Config", Config::self());
    auto dialog = new KConfigDialog(nullptr, u"godl settings"_s, Config::self());
    qmlRegisterSingletonInstance("org.fishy.godl", 0, 1, "ConfigDialog", dialog);
#endif
    qmlRegisterSingletonInstance("org.fishy.godl",
                                 0,
                                 1,
                                 "NetworkResponseCode",
                                 new NetworkResponseCode());
    QQmlApplicationEngine engine;
    Main::engine = &engine;

    Main::engine->rootContext()->setContextObject(new KLocalizedContext(&engine));
    Main::engine->loadFromModule(u"org.fishy.godl"_s, u"Main"_s);

    if (engine.rootObjects().isEmpty()) {
        Main::engine = nullptr;
        return -1;
    }

    auto return_code = app.exec();
    Main::engine = nullptr;
    return return_code;
}
