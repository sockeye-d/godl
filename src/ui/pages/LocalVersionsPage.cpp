#include "LocalVersionsPage.h"

#include <QListView>
#include <QScrollBar>
#include <QVBoxLayout>

#include "ui/widgets/carddelegate.h"
#include "ui/widgets/cardwidget.h"
#include "units.h"
#include "versionregistry.h"

namespace {
class VersionCardDelegate : public CardDelegate
{
    Q_OBJECT

protected:
    void setupCard(CardWidget *card) const override
    {
        auto showFolder = new QAction(QIcon::fromTheme("folder-open"), "Show folder");
        auto run = new QAction(QIcon::fromTheme("system-run"), "Run");
        auto editCommand = new QAction(QIcon::fromTheme("dialog-scripts"), "Edit command");
        auto showOnline = new QAction(QIcon::fromTheme("open-link"), "Show online");
        auto remove = new QAction(QIcon::fromTheme("delete"), "Remove");

        card->setMiddleHidden(true);
        card->addActions({showFolder, run, editCommand, showOnline, remove});
    }

    void updateCard(CardWidget *card, const QPersistentModelIndex &index) const override
    {
        card->title()->setText(getData<GodotVersion>(index)->toString());
    }

public:
    explicit VersionCardDelegate(QAbstractItemView *itemView, QObject *parent = nullptr)
        : CardDelegate(itemView, parent)
    {
        m_size = {0, Units::gridUnit * 5};
    }
};
} // namespace

LocalVersionsPage::LocalVersionsPage(QWidget *parent)
    : Page(parent)
{
    const auto layout = new QVBoxLayout(this);
    const auto view = new QListView(this);
    view->setModel(VersionRegistry::instance()->model());
    view->setItemDelegate(new VersionCardDelegate(view));
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    view->setAutoFillBackground(false);
    view->setSpacing(Units::smallSpacing);
    view->verticalScrollBar()->setSingleStep(20);
    layout->addWidget(view);
    layout->setContentsMargins(0, 0, 0, 0);
}

QString LocalVersionsPage::name()
{
    return "Local versions";
}

QString LocalVersionsPage::iconName()
{
    return "drive";
}

#include "LocalVersionsPage.moc"