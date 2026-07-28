#include "cardwidget.h"

#include <QApplication>
#include <QLabel>
#include <QPaintEvent>
#include <QVBoxLayout>

#include <QTimer>

#include "units.h"
#include <KColorScheme>

void CardWidget::actionEvent(QActionEvent *event)
{
    QApplication::sendEvent(m_toolbar, event);
}

QLabel *CardWidget::title() const
{
    return m_title;
}

QVBoxLayout *CardWidget::innerLayout() const
{
    return m_content;
}

QToolBar *CardWidget::toolbar() const
{
    return m_toolbar;
}

void CardWidget::setMiddleHidden(const bool hidden) const
{
    m_middleWidget->setHidden(!hidden);
}

CardWidget::CardWidget(QWidget *parent)
    : QPushButton(parent)
    , m_title{new QLabel(this)}
    , m_content{new QVBoxLayout()}
    , m_toolbar{new QToolBar()}
    , m_middleWidget{new QWidget()}
{
    setObjectName("CardWidget");

    const auto outer = new QVBoxLayout();
    setLayout(outer);
    outer->setSpacing(Units::smallSpacing);

    m_title->setFont(Units::heading(m_title->font(), 1));
    m_title->setContentsMargins(Units::smallSpacing, Units::smallSpacing, Units::smallSpacing, 0);
    outer->addWidget(m_title);

    m_middleWidget->setContentsMargins(Units::smallSpacing, 0, Units::smallSpacing, 0);
    m_middleWidget->setLayout(m_content);

    outer->addWidget(m_middleWidget, 1);

    const auto toolbarContainer = new QWidget();
    const auto toolbarLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    toolbarContainer->setLayout(toolbarLayout);

    m_toolbar = new QToolBar();
    toolbarLayout->addWidget(m_toolbar, 1, Qt::AlignLeft);

    m_toolbar->setToolButtonStyle(Qt::ToolButtonFollowStyle);

    outer->addWidget(toolbarContainer);
}

CardWidget::~CardWidget() = default;