#include "godlwindow.h"

#include <KToolBar>
#include <QActionGroup>
#include <QDockWidget>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QToolButton>

#include "pages/LocalVersionsPage.h"
#include "pages/ProjectsPage.h"
#include "pages/RemoteVersionsPage.h"
#include "versioninfo.h"
#include <qboxlayout.h>

void GodlWindow::activatePage(Page *page)
{
    if (centralWidget() == page)
        return;
    takeCentralWidget();
    setCentralWidget(page);

    for (const auto &action : pageActions) {
        toolbar->removeAction(action);
    }
    const auto newPageActions = page->actions();
    if (newPageActions.isEmpty()) {
        return;
    }
    toolbar->addSeparator();
    toolbar->addActions(newPageActions);
    pageActions = newPageActions;
}

GodlWindow::GodlWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
    , pages{
          new ProjectsPage(),
          new LocalVersionsPage(),
          new RemoteVersionsPage(),
      }
    , toolbar{new KToolBar(this, true)}
{
    const auto tabActionGroup = new QActionGroup(this);

    for (const auto &page : pages) {
        const auto action = new QAction(QIcon::fromTheme(page->iconName()), page->name(), this);
        action->setCheckable(true);
        action->setActionGroup(tabActionGroup);
        connect(action, &QAction::triggered, this, [page, this]() { activatePage(page); });
        builtinActions.append(action);
    }

    addToolBar(Qt::TopToolBarArea, toolbar);
    toolbar->addActions(builtinActions);
    builtinActions[1]->trigger();

    setupGUI(ToolBar | Keys | StatusBar | Save);

    const auto footer = new QWidget();

    const auto footerLayout = new QHBoxLayout(footer);
    auto margins = footerLayout->contentsMargins();
    margins.setBottom(0);
    margins.setTop(0);
    margins.setRight(0);
    footerLayout->setContentsMargins(margins);

    const auto versionLabel = new QLabel();
    versionLabel->setText(QString("%1 (%2)").arg(VersionInfo::instance()->tag(),
                                                 VersionInfo::instance()->commitHash()));
    footerLayout->addWidget(versionLabel);
    footerLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    const auto downloadsButton = new QPushButton();
    downloadsButton->setIcon(QIcon::fromTheme("download"));
    downloadsButton->setFlat(true);
    footerLayout->addWidget(downloadsButton);

    footer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    statusBar()->addWidget(footer, 1);
    statusBar()->setSizeGripEnabled(false);
}

GodlWindow::~GodlWindow()
{
    for (const auto &page : pages) {
        page->deleteLater();
    }
    pages.clear();
}
