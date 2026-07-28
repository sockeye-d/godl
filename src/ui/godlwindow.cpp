#include "godlwindow.h"

#include <KActionCollection>
#include <KStandardAction>
#include <KToolBar>
#include <QActionGroup>
#include <QDebug>
#include <QLabel>
#include <QTimer>

#include "pages/LocalVersionsPage.h"
#include "pages/ProjectsPage.h"
#include "pages/RemoteVersionsPage.h"
#include <qlayout.h>

GodlWindow::GodlWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
    , pages{
          new ProjectsPage(nullptr),
          new LocalVersionsPage(nullptr),
          new RemoteVersionsPage(nullptr),
      }
{
    const auto tabActionGroup = new QActionGroup(this);

    QList<QAction *> actions;

    for (const auto &page : pages) {
        const auto action = new QAction(QIcon::fromTheme(page->iconName()), page->name(), this);
        action->setCheckable(true);
        action->setActionGroup(tabActionGroup);
        connect(action, &QAction::triggered, this, [page, this]() {
            takeCentralWidget();
            page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            setCentralWidget(page);
        });
        actions.append(action);
    }

    actions[1]->trigger();

    const auto toolbar = new KToolBar(this, true);
    toolbar->addActions(actions);
    addToolBar(Qt::TopToolBarArea, toolbar);

    setupGUI(ToolBar | Keys | StatusBar | Save);
}

GodlWindow::~GodlWindow()
{
    for (const auto &page : pages) {
        page->deleteLater();
    }
    pages.clear();
}
