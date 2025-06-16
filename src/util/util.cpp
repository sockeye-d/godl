#include "util.h"
#include <QSysInfo>

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
