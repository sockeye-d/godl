#include "RemoteVersionsPage.h"

#include <QBoxLayout>
#include <QLabel>

RemoteVersionsPage::RemoteVersionsPage(QWidget *parent)
    : Page(parent)
{
    const auto layout = new QBoxLayout(QBoxLayout::LeftToRight);
    setLayout(layout);

    const auto label = new QLabel(this);
    label->setText("Remote versions page");
    layout->addWidget(label);
}

QString RemoteVersionsPage::name()
{
    return "Remote versions";
}

QString RemoteVersionsPage::iconName()
{
    return "globe";
}
