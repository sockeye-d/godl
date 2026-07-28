#include "cardwidget.h"

#include <QApplication>
#include <QLabel>
#include <QPaintEvent>
#include <QVBoxLayout>

#include <QTimer>

void CardWidget::actionEvent(QActionEvent *event)
{
    QApplication::sendEvent(m_toolbar, event);
}

QLabel *CardWidget::title() const
{
    return m_title;
}

QLayout *CardWidget::innerLayout() const
{
    return m_content;
}

CardWidget::CardWidget(QWidget *parent)
    : QPushButton(parent)
    , m_title{new QLabel(this)}
    , m_content{new QVBoxLayout()}
    , m_toolbar{new QToolBar()}
{
    const auto outer = new QVBoxLayout();
    setLayout(outer);

    QFont f = m_title->font();
    f.setBold(true);
    f.setPointSize(f.pointSize() + 1);
    m_title->setFont(f);
    m_title->setContentsMargins(12, 10, 12, 6);
    m_title->setText("Hi");
    outer->addWidget(m_title);

    const auto contentWrapper = new QWidget(this);
    contentWrapper->setContentsMargins(12, 0, 12, 0);
    contentWrapper->setLayout(m_content);

    outer->addWidget(contentWrapper, 1);

    const auto toolbarContainer = new QWidget();
    const auto toolbarLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    toolbarContainer->setLayout(toolbarLayout);

    m_toolbar = new QToolBar();
    toolbarLayout->addWidget(m_toolbar, 1, Qt::AlignLeft);

    // toolbar->addActions(actions);
    m_toolbar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    m_toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    QTimer::singleShot(0, m_toolbar, [this]() {
        // god please save us all
        m_toolbar->setMovable(true);
        m_toolbar->setMovable(false);
    });

    outer->addWidget(toolbarContainer);
}

CardWidget::~CardWidget() = default;