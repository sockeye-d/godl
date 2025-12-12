#include "projectsregistrymodel.h"
#include "godotproject.h"
#include <qnamespace.h>
#include <qtimer.h>

namespace {
int sortFilterPrecedence(const ProjectsRegistryModel *t, const GodotProject *item)
{
    if (t->filter().startsWith("tag:")) {
        auto tagFilter = t->filter().sliced(4);
        const QStringList &tags = item->tags();
        for (const QString &tag : tags) {
            if (tag == tagFilter) {
                return 4;
            }
        }
        return 0;
    }
    if (t->filterCaseInsensitive()) {
        if (item->name().toLower().contains(t->filter().toLower())) {
            return 3;
        }
        return 2;
    } else {
        if (item->name().contains(t->filter())) {
            return 1;
        }
        return 0;
    }
}
} // namespace

ProjectsRegistryModel::ProjectsRegistryModel(InternalProjectsRegistryModel *model, QObject *parent)
    : QSortFilterProxyModel{parent}
{
    setSourceModel(model);
    connect(model,
            &InternalProjectsRegistryModel::rowsInserted,
            this,
            [this](QModelIndex, int first, int) {
                auto obj = getProject(first);
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
                    connect(obj, &GodotProject::tagsChanged, this, &ProjectsRegistryModel::resort);
                }
            });
}

bool ProjectsRegistryModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent)
    if (filter() == "") {
        return true;
    }
    const GodotProject *item = getProject(source_row);
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
    if (filter().startsWith("exact:")) {
        auto exactFilter = filter().sliced(6);
        return item->name() == exactFilter;
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
    const GodotProject *a = getProject(source_left.row());
    const GodotProject *b = getProject(source_right.row());

    if (!a->favorite() && b->favorite()) {
        return !ascending();
    }

    if (a->favorite() && !b->favorite()) {
        return ascending();
    }

    if (!filter().isEmpty()) {
        auto l = sortFilterPrecedence(this, a);
        auto r = sortFilterPrecedence(this, b);
        if (r > l) {
            return !ascending();
        }
    }

    if (sortBy() == ModifiedDate) {
        return a->lastEditedTime() < b->lastEditedTime();
    }

    return a->name().toLower() < b->name().toLower();
}

const GodotProject *ProjectsRegistryModel::getProject(int index) const
{
    return static_cast<InternalProjectsRegistryModel *>(sourceModel())->m_data[index];
}

void ProjectsRegistryModel::resort()
{
    using namespace std::chrono_literals;
    resortInterval(0ms);
}

void ProjectsRegistryModel::resortInterval(std::chrono::milliseconds interval)
{
    auto timer = new QTimer();
    timer->setInterval(interval);
    connect(timer, &QTimer::timeout, this, [this, timer]() {
        sort(0, !ascending() ? Qt::AscendingOrder : Qt::DescendingOrder);
        sort(0, ascending() ? Qt::AscendingOrder : Qt::DescendingOrder);
        timer->deleteLater();
    });
    timer->start();
}
