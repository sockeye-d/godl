#ifndef VERSIONREGISTRYMODEL_H
#define VERSIONREGISTRYMODEL_H

#include <QAbstractListModel>
#include "abstractsimplelistmodel.h"
#include "godotversion.h"

class VersionRegistryModel : public AbstractSimpleListModel<std::shared_ptr<const GodotVersion>>
{
    Q_OBJECT
public:
    explicit VersionRegistryModel(QObject *parent = nullptr)
        : AbstractSimpleListModel<std::shared_ptr<const GodotVersion>>(parent) {};
};

#endif // VERSIONREGISTRYMODEL_H
