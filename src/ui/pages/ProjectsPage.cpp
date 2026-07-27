#include "ProjectsPage.h"

#include <QBoxLayout>
#include <QLabel>

ProjectsPage::ProjectsPage(QWidget *parent)
    : Page(parent)
{
    const auto layout = new QBoxLayout(QBoxLayout::LeftToRight);
    setLayout(layout);

    const auto label = new QLabel(this);
    label->setText("Projects page");
    layout->addWidget(label);
}

QString ProjectsPage::name()
{
    return "Projects";
}

QString ProjectsPage::iconName()
{
    return "document-edit";
}
