#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <QDir>
#include <QStringList>

QFileInfoList getEntries(const QString &path, QDir::Filters filters);
QString getPathRoot(const QString &path);

#endif // FILEUTIL_H
