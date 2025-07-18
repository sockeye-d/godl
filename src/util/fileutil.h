#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <QDir>
#include <QStringList>

QFileInfoList getEntries(const QString &path, QDir::Filters filters);

#endif // FILEUTIL_H
