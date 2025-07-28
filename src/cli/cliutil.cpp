#include "cliutil.h"

#include <QDir>
#include "util.h"

QString cli::makeAbsolute(const QString &path)
{
    QDir dir = QDir::current();
    dir.setPath(dir.absolutePath() / path);
    return dir.absolutePath();
}
