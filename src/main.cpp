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

int main(int argc, char *argv[])
{
    KIconTheme::initTheme();
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme("godl"));

    KLocalizedString::setApplicationDomain("godl");
    QApplication::setOrganizationName(QStringLiteral("fishy"));
    QApplication::setOrganizationDomain(QStringLiteral("fishy.org"));
    QApplication::setApplicationName(QStringLiteral("godl"));
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
    qmlRegisterType<Template>("org.fishy.godl", 0, 1, "projectTemplate");
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
    engine.loadFromModule(u"org.fishy.godl"_s, u"Main"_s);

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
