#include "projectsregistrymodel.h"
#include "godotproject.h"
#include "util.h"
#include <qnamespace.h>
#include <qtimer.h>

ProjectsRegistryModel::ProjectsRegistryModel(InternalProjectsRegistryModel *model, QObject *parent)
    : QSortFilterProxyModel{parent}
{
    setSourceModel(model);
    connect(model,
            &InternalProjectsRegistryModel::rowsInserted,
            this,
            [this](QModelIndex, int first, int) {
                auto obj = project(first);
                if (obj) {
                    connect(obj,
                            &GodotProject::favoriteChanged,
                            this,
                            &ProjectsRegistryModel::resort);
                    connect(obj, &GodotProject::nameChanged, this, &ProjectsRegistryModel::resort);
                    connect(obj,
                            &GodotProject::descriptionChanged,
                            this,
                            &ProjectsRegistryModel::resort);
                    connect(obj,
                            &GodotProject::lastEditedTimeChanged,
                            this,
                            &ProjectsRegistryModel::resort);
                    connect(obj,
                            &GodotProject::tagsChanged,
                            this,
                            &ProjectsRegistryModel::invalidateFilter);
                }
            });
}

bool ProjectsRegistryModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)
    if (filter() == "") {
        return true;
    }
    const GodotProject *item = project(source_row);
    if (filter().startsWith("tag:")) {
        auto tagFilter = filter().sliced(4);
        const QStringList &tags = item->tags();
        for (const QString &tag : tags) {
            if (tag == tagFilter) {
                return true;
            }
        }
        return false;
    }
    if (filterCaseInsensitive()) {
        return item->name().toLower().contains(filter().toLower())
               || item->description().toLower().contains(filter().toLower());
    } else {
        return item->name().contains(filter()) || item->description().contains(filter());
    }
}

bool ProjectsRegistryModel::lessThan(const QModelIndex &source_left,
                                     const QModelIndex &source_right) const
{
    const GodotProject *a = project(source_left.row());
    const GodotProject *b = project(source_right.row());

    if (!a->favorite() && b->favorite()) {
        return !ascending();
    }

    if (a->favorite() && !b->favorite()) {
        return ascending();
    }

    if (sortBy() == ModifiedDate) {
        return a->lastEditedTime() < b->lastEditedTime();
    }

    return a->name().toLower() < b->name().toLower();
}

const GodotProject *ProjectsRegistryModel::project(int index) const
{
    return static_cast<InternalProjectsRegistryModel *>(sourceModel())->m_data[index];
}

void ProjectsRegistryModel::resort()
{
    debug() << "resorting";
    auto timer = new QTimer();
    timer->setInterval(0);
    connect(timer, &QTimer::timeout, this, [this, timer]() {
        sort(0, !ascending() ? Qt::AscendingOrder : Qt::DescendingOrder);
        sort(0, ascending() ? Qt::AscendingOrder : Qt::DescendingOrder);
        timer->deleteLater();
    });
    timer->start();
}
