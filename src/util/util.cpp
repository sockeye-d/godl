#include "util.h"

QString joinPath(const QString &a, const QString &b)
{
    if (a.endsWith(u"/")) {
        return a + b;
    }
    return a + "/" + b;
}
