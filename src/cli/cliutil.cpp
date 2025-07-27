#include "cliutil.h"

#include <QDir>

QString cli::makeAbsolute(const QString &path)
{
    QDir dir = QDir::current();
    dir.cd(path);
    return dir.absolutePath();
}
