#include "fileutil.h"

#include <QDirIterator>

QFileInfoList getEntries(const QString &path, QDir::Filters filters)
{
    QDirIterator it(path, filters);
    QFileInfoList results{};
    while (it.hasNext())
        results.append(it.nextFileInfo());
    return results;
}
