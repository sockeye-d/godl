#ifndef PROJECTSREGISTRYMODEL_H
#define PROJECTSREGISTRYMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include "abstractsimplelistmodel.h"
#include "godotproject.h"

class ProjectsRegistryModel : public AbstractSimpleListModel<std::shared_ptr<const GodotProject>>
{
    Q_OBJECT
public:
    explicit ProjectsRegistryModel(QObject *parent = nullptr)
        : AbstractSimpleListModel<std::shared_ptr<const GodotProject>>(parent)
    {}
};

#endif // PROJECTSREGISTRYMODEL_H
