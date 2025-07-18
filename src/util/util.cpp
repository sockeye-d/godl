#include "util.h"
#include <QDirIterator>
#include <QFile>
#include <QStandardPaths>
#include <QSysInfo>
#include <KTar>
#include <KZip>

using namespace Qt::Literals::StringLiterals;

QString joinPath(const QString &a, const QString &b)
{
    if (a.endsWith(u"/")) {
        return a + b;
    }
    return a + "/" + b;
}

QStringList sysInfo()
{
    auto os = QSysInfo::kernelType();
    auto arch = QSysInfo::currentCpuArchitecture();
    if (arch == "arm") {
        arch = "arm32";
    } else if (arch == "i386") {
        arch = "x86_32";
    }
    if (os == "linux") {
        return {"linux_" % arch, "linux." % arch, "x11"};
    } else if (os == "darwin") {
        // by the way this will never run on mac this is just for completeness
        return {"macos"};
    } else if (os == "android") {
        return {"android_editor"};
    } else if (os == "winnt") {
        if (arch == "x86_32") {
            return {"win32"};
        } else if (arch == "x86_64") {
            return {"win64"};
        } else {
            // why arm isn't winarm32/winarm64 boggles the mind
            return {"windows_" % arch};
        }
    }
    return {};
}

QString operator/(const QString &a, const QString &b)
{
    return joinPath(a, b);
}

std::unique_ptr<KArchive> openArchive(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file to peek at the header";
        return {};
    }

    std::unique_ptr<KArchive> archive;
    if (file.peek(2).startsWith("\x50\x4B")) {
        // ah it must be a zip
        debug() << "Detected as zip";
        archive = std::make_unique<KZip>(filePath);
    } else if (filePath.endsWith(u".tar.xz"_s)) {
        debug() << "Detected as xz-ed tarball";
        archive = std::make_unique<KTar>(filePath);
    }

    if (!archive->open(QIODevice::ReadWrite)) {
        return {};
    }

    return archive;
}

QString getDirNameFromFilePath(const QString &filepath)
{
    using namespace Qt::StringLiterals;
    auto dirsplit = filepath.split(u"/"_s);
    if (dirsplit.isEmpty()) {
        return filepath;
    }
    auto filesplit = dirsplit.last().split(u"."_s);
    if (filesplit.isEmpty()) {
        return dirsplit.last();
    }

    return filesplit.sliced(0, filesplit.size() - 1).join(u"."_s);
}

QString removePrefix(const QString &string, const QString &prefix)
{
    if (!string.startsWith(prefix)) {
        return string;
    }

    return string.sliced(prefix.length());
}

/**
 * @brief Ensures the path has a '/' at the end. If it has one, nothing is
 * changed, but if it doesn't, a slash is appended. This doesn't handle any sort
 * of resolution of dots and dot-dots.
 * @param dirpath The path to normalize.
 * @return The normalized path.
 */
QString normalizeDirectoryPath(const QString &dirpath)
{
    if (dirpath.endsWith("/")) {
        return dirpath;
    }

    return dirpath + "/";
}

QString findGodotProjectLocation()
{
    auto path = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).constFirst()
                / "godot/projects";
    if (QDir(path).exists()) {
        return path;
    }
    return QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).constFirst();
}
