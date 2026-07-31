#include "godlwindow.h"

#include <KToolBar>
#include <QActionGroup>
#include <QDockWidget>
#include <QGraphicsAnchorLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStackedLayout>
#include <QStatusBar>
#include <QToolButton>

#include "downloadmanager.h"
#include "downloadmanagermodel.h"
#include "pages/LocalVersionsPage.h"
#include "pages/ProjectsPage.h"
#include "pages/RemoteVersionsPage.h"
#include "units.h"
#include "versioninfo.h"
#include "widgets/cardwidget.h"
#include "widgets/downloadspopup.h"
#include <KColorScheme>
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
    , toolbar{new KToolBar(this, true)}
    , m_layout{new QStackedLayout()}
{
    const auto projectsPage = new ProjectsPage();
    const auto localVersionsPage = new LocalVersionsPage();
    const auto remoteVersionsPage = new RemoteVersionsPage();

    pages = {projectsPage, localVersionsPage, remoteVersionsPage};

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
    versionLabel->setText(QString("%1 (%2)").arg(VersionInfo::instance()->tag(), VersionInfo::instance()->commitHash()));
    footerLayout->addWidget(versionLabel);
    footerLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    const auto downloadsTotalProgress = new QProgressBar();
    downloadsTotalProgress->setMinimumWidth(100);
    downloadsTotalProgress->setTextVisible(false);

    footerLayout->addWidget(downloadsTotalProgress, 0, Qt::AlignRight);

    const auto downloadsButton = new QPushButton();
    downloadsButton->setIcon(QIcon::fromTheme("download"));
    downloadsButton->setFlat(true);
    downloadsButton->setCheckable(true);
    footerLayout->addWidget(downloadsButton);

    const auto downloadsPopup = new DownloadsPopup(downloadsButton, this);
    downloadsPopup->hide();

    const auto updateTotalProgress = [downloadsTotalProgress](const int progress, const int progressTotal) {
        if (progress == -1) {
            downloadsTotalProgress->hide();
            return;
        }
        downloadsTotalProgress->show();

        QPalette palette;
        if (progress < 0) {
            const KColorScheme scheme{};
            QColor color;
            if (progress == -3) {
                // Done
                color = scheme.foreground(KColorScheme::PositiveText).color();
            }
            if (progress == -2) {
                // Error
                color = scheme.foreground(KColorScheme::NegativeText).color();
            }
            palette.setColor(QPalette::Accent, color);
            palette.setColor(QPalette::Highlight, color);
            downloadsTotalProgress->setRange(0, 1);
            downloadsTotalProgress->setValue(1);
        } else {
            downloadsTotalProgress->setStyleSheet("");
            downloadsTotalProgress->setRange(0, progressTotal);
            downloadsTotalProgress->setValue(progress);
        }

        downloadsTotalProgress->setPalette(palette);
    };

    updateTotalProgress(0, 0);
    connect(DownloadManager::instance(), &DownloadManager::progressChanged, this, updateTotalProgress);

    auto updateRowCount = [downloadsButton] {
        downloadsButton->setEnabled(downloadsButton->isChecked() || DownloadManager::instance()->model()->rowCount(QModelIndex()) > 0);
    };

    connect(DownloadManager::instance()->model(), &QAbstractItemModel::rowsInserted, this, updateRowCount);
    connect(DownloadManager::instance()->model(), &QAbstractItemModel::rowsRemoved, this, updateRowCount);
    updateRowCount();

    connect(downloadsButton, &QAbstractButton::clicked, this, updateRowCount);

    connect(remoteVersionsPage, &RemoteVersionsPage::downloadStarted, this, [downloadsButton, downloadsPopup] {
        downloadsButton->setChecked(true);
        downloadsPopup->popup();
    });

    footer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    statusBar()->addWidget(footer, 1);
    statusBar()->setSizeGripEnabled(false);
}
