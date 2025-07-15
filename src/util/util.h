#ifndef UTIL_H
#define UTIL_H

#include <QList>
#include <QVariant>
#include <KArchive>

template<typename T>
struct QObjDeleter
{
    static_assert(std::is_base_of<QObject, T>(), "T must be a subclass of QObject");
    void operator()(T *ptr) const { ptr->deleteLater(); }
};

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

QString getDirNameFromFilePath(const QString &filepath);

QStringList sysInfo();

std::unique_ptr<KArchive> openArchive(const QString &filePath);

QString removePrefix(const QString &string, const QString &prefix);

QString normalizeDirectoryPath(const QString &dirpath);

QString findGodotProjectLocation();

/**
 * @brief make_shared_qobj Like std::make_shared but uses QObject::deleteLater instead of @ref delete
 * @param args The arguments to forward to the constructor
 * @return A new shared_ptr 
 */
template<typename T, typename... Args>
inline std::shared_ptr<T> make_shared_qobj(Args &&...args)
{
    return std::shared_ptr<T>(new T(std::forward<Args>(args)...), QObjDeleter<T>{});
}

/**
 * @brief make_shared_qobj Like std::make_shared but uses QObject::deleteLater instead of @ref delete
 * @param args The arguments to forward to the constructor
 * @return A new shared_ptr 
 */
template<typename T>
inline std::shared_ptr<T> shared_qobj(T *ptr)
{
    return std::shared_ptr<T>(ptr, QObjDeleter<T>{});
}

/**
 * @brief make_unique_qobj Like std::make_unique but uses QObject::deleteLater instead of @ref delete
 * @param args The arguments to forward to the constructor
 * @return A new unique_ptr 
 */
template<typename T, typename... Args>
inline std::unique_ptr<T, QObjDeleter<T>> make_unique_qobj(Args &&...args)
{
    return std::unique_ptr<T, QObjDeleter<T>>(new T(std::forward<Args>(args)...), QObjDeleter<T>{});
}

/**
 * @brief make_unique_qobj Like std::make_unique but uses QObject::deleteLater instead of @ref delete
 * @param args The arguments to forward to the constructor
 * @return A new unique_ptr 
 */
template<typename T>
inline std::unique_ptr<T, QObjDeleter<T>> unique_qobj(T *ptr)
{
    return std::unique_ptr<T, QObjDeleter<T>>(ptr, QObjDeleter<T>{});
}

template<typename T>
using qobj_unique_ptr = std::unique_ptr<T, QObjDeleter<T>>;

#ifdef DEBUG
#define debug() qDebug() << QString(__FILE__) % "@" % QString::number(__LINE__) % QString(":")
#else
#define debug() qDebug()
#endif

#endif // UTIL_H
