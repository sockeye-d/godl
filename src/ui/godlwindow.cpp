#include "godlwindow.h"

#include <KToolBar>
#include <QActionGroup>
#include <QDockWidget>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <QStatusBar>
#include <QToolButton>

#include "pages/LocalVersionsPage.h"
#include "pages/ProjectsPage.h"
#include "pages/RemoteVersionsPage.h"
#include "units.h"
#include "versioninfo.h"
#include <qboxlayout.h>

void GodlWindow::activatePage(Page *page)
{
    m_layout->setCurrentWidget(page);

    for (const auto &action : pageActions) {
        toolbar->removeAction(action);
    }

    pageActions.clear();

    const auto newPageActions = page->actions();
    if (newPageActions.isEmpty()) {
        return;
    }

    pageActions.append(toolbar->addSeparator());

    for (const auto &actionVariant : newPageActions) {
        if (const auto action = std::get_if<QAction *>(&actionVariant)) {
            pageActions.append(*action);
            toolbar->addAction(*action);
        } else if (const auto widgetPtr = std::get_if<QWidget *>(&actionVariant)) {
            const auto widget = *widgetPtr;
            widget->setSizePolicy(widget->sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);

            pageActions.append(toolbar->addWidget(widget));
        }
    }
}

GodlWindow::GodlWindow(QWidget *parent)
    : QMainWindow(parent, Qt::Window)
    , pages{
          new ProjectsPage(),
          new LocalVersionsPage(),
          new RemoteVersionsPage(),
      }
    , toolbar{new KToolBar(this, true)}
    , m_layout{new QStackedLayout()}
{
    setMinimumSize(Units::gridUnit * 60, Units::gridUnit * 45);
    const auto tabActionGroup = new QActionGroup(this);

    for (const auto &page : pages) {
        const auto action = new QAction(QIcon::fromTheme(page->iconName()), page->name(), this);
        action->setCheckable(true);
        action->setActionGroup(tabActionGroup);
        connect(action, &QAction::triggered, this, [page, this]() { activatePage(page); });
        builtinActions.append(action);

        m_layout->addWidget(page);
    }

    const auto centralWidget = new QWidget();
    centralWidget->setLayout(m_layout);
    setCentralWidget(centralWidget);

    addToolBar(Qt::TopToolBarArea, toolbar);
    toolbar->addActions(builtinActions);

    m_layout->setCurrentWidget(pages[2]);
    builtinActions[2]->trigger();

    // setupGUI(ToolBar | Keys | StatusBar | Save);

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

// GodlWindow::~GodlWindow()
// {
//     for (const auto &page : pages) {
//         delete page;
//     }
//     pages.clear();
// }
