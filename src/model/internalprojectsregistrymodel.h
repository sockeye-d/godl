#ifndef INTERNALPROJECTSREGISTRYMODEL_H
#define INTERNALPROJECTSREGISTRYMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include "abstractsimplelistmodel.h"
#include "godotproject.h"

class InternalProjectsRegistryModel : public AbstractSimpleListModel<const GodotProject *>
{
    friend class ProjectsRegistryModel;
    Q_OBJECT
public:
    explicit InternalProjectsRegistryModel(QObject *parent = nullptr)
        : AbstractSimpleListModel<const GodotProject *>(parent)
    {}
};

#endif // INTERNALPROJECTSREGISTRYMODEL_H
