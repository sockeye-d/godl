#ifndef REMOTEVERSIONSPAGE_H
#define REMOTEVERSIONSPAGE_H

#include <QLabel>
#include <QPointer>

#include "ui/page.h"

class KWidgetItemDelegate;
class QAbstractItemDelegate;
class QSortFilterProxyModel;
class QLineEdit;
class QProgressBar;
class VersionListModel;

class RemoteVersionsPage : public Page
{
    friend class VersionListModel;
    Q_OBJECT

    QSortFilterProxyModel *m_sorter{};
    KWidgetItemDelegate *m_delegate{};
    QLabel *m_errorIconLabel{};
    QLabel *m_errorLabel{};
    QProgressBar *m_bar{};
    QWidget *m_barWrapper{};
    VersionListModel *m_versionListModel{};
    QPointer<QTimer> m_filterTimer{};

public:
    static QString getUrlForRepo(const QString &repo);
    explicit RemoteVersionsPage(QWidget *parent = nullptr);
    ~RemoteVersionsPage() override;

    void hideIndicators() const;
    void setProgress(int currentPage, int maxPages) const;
    void setError(const QString &errorText) const;

    QString name() override;
    QString iconName() override;
    QList<std::variant<QAction *, QWidget *>> actions() override;
};

#endif //REMOTEVERSIONSPAGE_H
