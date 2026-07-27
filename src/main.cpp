#include "cli/run.h"
#include "versioninfo.h"
#include <KAboutData>
#include <QApplication>

#if __has_include("config.h")
#define CONFIG
#include "config.h"
#endif

#include "main.h"
#include "projecttemplates.h"
#include "versionregistry.h"
#include <QtSystemDetection>

#include "ui/godlwindow.h"

int main(int argc, char *argv[])
{
    using namespace Qt::Literals::StringLiterals;

    if (argc > 1) {
        return cli::run::runCli(argc, argv);
    }

//     if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
// #ifdef Q_OS_LINUX
//         QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
// #elif defined(Q_OS_MACOS)
//         QQuickStyle::setStyle(QStringLiteral("macOS"));
// #else
//         QQuickStyle::setStyle(QStringLiteral("FluentWinUI3"));
// #endif
//     }

    // KIconTheme::initTheme();

    QApplication app(argc, argv);
    QApplication::setStyle(QStringLiteral("Breeze"));
    QApplication::setWindowIcon(QIcon::fromTheme("godl"));

    // KLocalizedString::setApplicationDomain("godl");
    cli::run::setAppMetadata();
    QApplication::setDesktopFileName(QStringLiteral("dev.fishies.godl"));
    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/");

    ProjectTemplates::instance()->extractDefault();

    KAboutData aboutData(QStringLiteral("godl"),
                         "godl",
                         VersionInfo::instance()->tag(),
                         "Godot version manager and downloader",
                         KAboutLicense::MIT,
                         "(c) 2025");

    aboutData.setHomepage(QStringLiteral("https://github.com/sockeye-d/godl"));
    aboutData.setBugAddress("https://github.com/sockeye-d/godl/issues/new"_ba);
    aboutData.setDesktopFileName("dev.fishies.godl");

    KAboutData::setApplicationData(aboutData);

    GodlWindow window{};
    window.show();
    const int returnCode = app.exec();

    if (returnCode == 0)
        Config::self()->save();

    return returnCode;
}
