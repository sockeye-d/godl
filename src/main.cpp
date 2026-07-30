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
#include <QStyleFactory>

#include "ui/godlwindow.h"

int main(int argc, char *argv[])
{
    using namespace Qt::Literals::StringLiterals;

    if (argc > 1) {
        return cli::run::runCli(argc, argv);
    }

    QApplication app(argc, argv);
    QApplication::setWindowIcon(QIcon::fromTheme("godl"));

    // KLocalizedString::setApplicationDomain("godl");
    cli::run::setAppMetadata();
    QApplication::setDesktopFileName(QStringLiteral("dev.fishies.godl"));
    QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths() << ":/");

    ProjectTemplates::instance()->extractDefault();

    KAboutData aboutData(QStringLiteral("godl2"),
                         "godl2",
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

    const int returnCode = QApplication::exec();

    if (returnCode == 0)
        Config::self()->save();

    return returnCode;
}
