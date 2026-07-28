#include "LocalVersionsPage.h"

#include <QListView>
#include <QScrollBar>

#include "ui/widgets/carddelegate.h"
#include "versionregistry.h"
#include <KColorScheme>

LocalVersionsPage::LocalVersionsPage(QWidget *parent)
    : Page(parent)
{
    const KColorScheme colorScheme{};

    setLayout(new QVBoxLayout);
    const auto view = new QListView(this);
    auto model = VersionRegistry::instance()->model();
    view->setModel(model);
    view->setItemDelegate(new CardDelegate(view));
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    view->setAutoFillBackground(false);
    layout()->addWidget(view);

    for (int row = 0; row < model->rowCount(QModelIndex()); ++row)
        view->openPersistentEditor(model->index(row, 0));

    // Open for new rows as they're added
    connect(model,
            &QAbstractItemModel::rowsInserted,
            [view](const QModelIndex &parent, int first, int last) {
                for (int row = first; row <= last; ++row)
                    view->openPersistentEditor(view->model()->index(row, 0, parent));
            });
}

QString LocalVersionsPage::name()
{
    return "Local versions";
}

QString LocalVersionsPage::iconName()
{
    return "drive";
}
