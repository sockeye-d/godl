#ifndef VERSIONREGISTRYMODEL_H
#define VERSIONREGISTRYMODEL_H

#include <QAbstractListModel>
#include "abstractsimplelistmodel.h"
#include "godotversion.h"

class VersionRegistryModel : public AbstractSimpleListModel<const GodotVersion *>
{
    Q_OBJECT
public:
    explicit VersionRegistryModel(QObject *parent = nullptr)
        : AbstractSimpleListModel<const GodotVersion *>(parent) {};
};

#endif // VERSIONREGISTRYMODEL_H
