#ifndef UTIL_H
#define UTIL_H

#include <QList>
#include <QVariant>
#include <KArchive>

template<typename T>
QList<T> convertList(QVariantList list) {
    QList<T> newList;
    for (const auto &element: list) {
        newList.append(element.value<T>());
    }
    return newList;
}

QString joinPath(const QString &a, const QString &b);

QString operator/(const QString &a, const QString &b);

QStringList sysInfo();

std::unique_ptr<KArchive> openArchive(const QString &filePath);

#endif // UTIL_H
