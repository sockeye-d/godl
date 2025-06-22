#include "util.h"
#include <QFile>
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
        return {"linux_" % arch, "linux." % arch};
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
        } else if (arch == "arm32") {
            // why arm isn't winarm32/winarm64 boggles the mind
            return {"windows_arm32"};
        } else if (arch == "arm64") {
            return {"windows_arm64"};
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
        qWarning() << "Failed to open archive";
        return nullptr;
    }
    if (file.peek(2).startsWith("\x50\x4B")) {
        // ah it must be a zip
        auto archive = std::make_unique<KZip>(filePath);
        archive->open(QIODevice::ReadOnly);
        return archive;
    }

    return nullptr;
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
