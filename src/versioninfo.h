#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include <QObject>

class VersionInfo : public QObject
{
    Q_OBJECT
public:
    explicit VersionInfo(QObject *parent = nullptr);

signals:
};

#endif // VERSIONINFO_H
