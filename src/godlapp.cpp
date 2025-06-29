#include "godlapp.h"

#include <KAuthorized>

void GodlApp::setupActions()
{
    AbstractKirigamiApplication::setupActions();

    auto nextTab = new QAction();
    nextTab->setText("Next page");
    nextTab->setObjectName("godl-next-page");
    nextTab->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Tab));
    mainCollection()->addAction(nextTab->objectName(), nextTab);

    auto prevTab = new QAction();
    prevTab->setText("Previous page");
    prevTab->setObjectName("godl-prev-page");
    prevTab->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab));
    mainCollection()->addAction(prevTab->objectName(), prevTab);

    // mainCollection()->addActions({nextTab, prevTab});
    // mainCollection()->setDefaultShortcut(nextTab, QKeySequence(Qt::CTRL | Qt::Key_Tab));
    // mainCollection()->setDefaultShortcut(prevTab, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab));

    // auto actionName = QLatin1String("add_notebook");
    // if (KAuthorized::authorizeAction(actionName)) {
    //     auto action = mainCollection()->addAction(actionName, this, []() { qDebug() << "hi"; });
    //     action->setText("New Notebook");
    //     action->setIcon(QIcon::fromTheme(QStringLiteral("list-add-symbolic")));
    //     mainCollection()->addAction(action->objectName(), action);
    //     mainCollection()->setDefaultShortcut(action, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N));
    // }
}
