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
}

QString LocalVersionsPage::name()
{
    return "Local versions";
}

QString LocalVersionsPage::iconName()
{
    return "drive";
}
