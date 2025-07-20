#ifndef PROJECTSREGISTRYMODEL_H
#define PROJECTSREGISTRYMODEL_H

#include "internalprojectsregistrymodel.h"
#include "util.h"

// I'm going to <omitted> lose it
class ProjectsRegistry;

#include <QSortFilterProxyModel>

class ProjectsRegistryModel : public QSortFilterProxyModel
{
    friend class ProjectsRegistry;
    Q_OBJECT
private:
    const GodotProject *project(int index) const;
    void resort();

public:
    enum SortBy {
        Name,
        ModifiedDate,
    };
    Q_ENUM(SortBy)
private:
    Q_PROPERTY(SortBy sortBy READ sortBy WRITE setSortBy NOTIFY sortByChanged FINAL)
    SortBy m_sortBy = ModifiedDate;

public:
    void setSortBy(SortBy sortBy)
    {
        if (m_sortBy == sortBy)
            return;
        m_sortBy = sortBy;
        resort();
        Q_EMIT sortByChanged();
    }

    SortBy sortBy() const { return m_sortBy; }

    Q_SIGNAL void sortByChanged();

private:
    Q_PROPERTY(bool ascending READ ascending WRITE setAscending NOTIFY ascendingChanged FINAL)
    bool m_ascending = false;

public:
    void setAscending(bool ascending)
    {
        if (m_ascending == ascending)
            return;
        m_ascending = ascending;
        resort();
        Q_EMIT ascendingChanged();
    }

    bool ascending() const { return m_ascending; }

    Q_SIGNAL void ascendingChanged();

private:
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged FINAL)
    QString m_filter = "";

public:
    void setFilter(QString filter)
    {
        if (m_filter == filter)
            return;
        beginFilterChange();
        m_filter = filter;
        invalidateFilter();
        resort();
        Q_EMIT filterChanged();
    }

    QString filter() const { return m_filter; }

    Q_SIGNAL void filterChanged();

private:
    Q_PROPERTY(bool filterCaseInsensitive READ filterCaseInsensitive WRITE setFilterCaseInsensitive
                   NOTIFY filterCaseInsensitiveChanged FINAL)
    bool m_filterCaseInsensitive = true;

public:
    void setFilterCaseInsensitive(bool filterCaseInsensitive)
    {
        if (m_filterCaseInsensitive == filterCaseInsensitive)
            return;
        m_filterCaseInsensitive = filterCaseInsensitive;
        invalidateFilter();
        Q_EMIT filterCaseInsensitiveChanged();
    }

    bool filterCaseInsensitive() const { return m_filterCaseInsensitive; }

    Q_SIGNAL void filterCaseInsensitiveChanged();

public:
    explicit ProjectsRegistryModel(InternalProjectsRegistryModel *model, QObject *parent = nullptr);

    // QSortFilterProxyModel interface
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};

#endif // PROJECTSREGISTRYMODEL_H
