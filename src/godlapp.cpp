#include "godlapp.h"

#include <KAuthorized>

void GodlApp::setupActions()
{
    AbstractKirigamiApplication::setupActions();
    auto nextTab = new QAction();
    nextTab->setText("Next page");
    mainCollection()->addAction("godl_next_page", nextTab);
    mainCollection()->setDefaultShortcut(nextTab, QKeySequence(Qt::CTRL | Qt::Key_Tab));

    auto prevTab = new QAction();
    prevTab->setText("Previous page");
    mainCollection()->addAction("godl_prev_page", prevTab);
    mainCollection()->setDefaultShortcut(prevTab, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab));

    connect(nextTab, &QAction::triggered, this, [this](auto) { Q_EMIT nextTabTriggered(); });
    connect(prevTab, &QAction::triggered, this, [this](auto) { Q_EMIT prevTabTriggered(); });
}

GodlApp::GodlApp(QObject *parent)
    : AbstractKirigamiApplication(parent)
{
    GodlApp::setupActions();
}
