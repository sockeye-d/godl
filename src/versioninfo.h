#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include <QObject>
#include "singleton.h"
#include "util.h"

class VersionInfo : public QObject, public Singleton<VersionInfo>
{
    Q_OBJECT
    Q_PROPERTY(QString commitHash READ commitHash CONSTANT FINAL)
    Q_PROPERTY(QString tag READ tag CONSTANT FINAL)
    Q_PROPERTY(QString version READ version CONSTANT FINAL)

public:
    explicit VersionInfo(QObject *parent = nullptr);

    const QString &commitHash()
    {
        static QString hash = GIT_COMMIT_HASH;
        return hash;
    }

    const QString &tag()
    {
        static QString tag = GIT_TAG;
        return tag;
    }

    const QString &version()
    {
        static QString version = removePrefix(tag(), "v");
        return version;
    }
};

#endif // VERSIONINFO_H
