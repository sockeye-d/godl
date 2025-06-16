#ifndef UTIL_H
#define UTIL_H

#include <QVariant>
#include <QList>

template<typename T>
QList<T> convertList(QVariantList list) {
    QList<T> newList;
    for (const auto &element: list) {
        newList.append(element.value<T>());
    }
    return newList;
}

QString joinPath(const QString &a, const QString &b);

QStringList sysInfo();

#endif // UTIL_H
