#ifndef VERSIONREGISTRYMODEL_H
#define VERSIONREGISTRYMODEL_H

#include <QAbstractListModel>
#include "abstractsimplelistmodel.h"
#include "godotversion.h"

class VersionRegistryModel : public AbstractSimpleListModel
{
    Q_OBJECT
public:
    explicit VersionRegistryModel(QObject *parent = nullptr)
        : AbstractSimpleListModel(parent) {};
};

#endif // VERSIONREGISTRYMODEL_H
