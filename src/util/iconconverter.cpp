#include "iconconverter.h"

#include <KIconLoader>

IconConverter::IconConverter(QObject *parent)
    : QObject{parent}
{}

QUrl IconConverter::path(QString name, int size) const
{
    return QUrl::fromLocalFile(KIconLoader::global()->iconPath(name, -size));
}
