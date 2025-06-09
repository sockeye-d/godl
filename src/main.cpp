#include <KAboutData>
#include <KIconTheme>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <QApplication>
#include <QMainWindow>
#include <QQuickStyle>
#include <QtQml>
#include <KSharedConfig>
#include "config.h"
#include <chainedjsonrequest.h>
#include <downloadmanager.h>
#include "main.h"

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[]) {
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

    KAboutData aboutData(QStringLiteral("godl"), i18nc("@title", "godl"),
                         QStringLiteral("0.1"),
                         i18n("Godot version manager and downloader"),
                         KAboutLicense::MIT, i18n("(c) 2025"));

    KAboutData::setApplicationData(aboutData);

    qmlRegisterSingletonType("org.fishy.godl", 0, 1, "About",
                             [](QQmlEngine *engine, QJSEngine *) -> QJSValue {
        return engine->toScriptValue(
            KAboutData::applicationData());
    });
    qmlRegisterType<ChainedJsonRequest>("org.fishy.godl", 0, 1,
                                        "ChainedJsonRequest");
    qmlRegisterType<DownloadManager>("org.fishy.godl", 0, 1, "DownloadManager");
    qmlRegisterSingletonInstance("org.fishy.godl", 0, 1, "Config", Config::self());
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
