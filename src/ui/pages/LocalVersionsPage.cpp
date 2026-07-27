#include "LocalVersionsPage.h"

#include <QBoxLayout>
#include <QLabel>

LocalVersionsPage::LocalVersionsPage(QWidget *parent)
    : Page(parent)
{
    const auto layout = new QBoxLayout(QBoxLayout::LeftToRight);
    setLayout(layout);

    const auto label = new QLabel(this);
    label->setText("Local versions page");
    layout->addWidget(label);
}

QString LocalVersionsPage::name()
{
    return "Local versions";
}

QString LocalVersionsPage::iconName()
{
    return "drive";
}
