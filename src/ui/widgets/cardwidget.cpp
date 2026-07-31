#include "cardwidget.h"

#include <QLabel>
#include <QVBoxLayout>

#include <KColorScheme>
#include <QActionEvent>
#include <QPainter>
#include <QTimer>

#include "units.h"

void CardWidget::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);

    if (m_highlightMode != HighlightNone) {
        const KColorScheme scheme{};

        const QBrush color = scheme.foreground(
            static_cast<KColorScheme::ForegroundRole>(static_cast<int>(m_highlightMode) + KColorScheme::NegativeText - HighlightNegative));

        QPainter painter{this};
        painter.setRenderHints(QPainter::Antialiasing);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(color, 2.0));
        // cursed cursed hack never do this
        // without it though the 1px line isn't pixel snapped >:(
        painter.scale(0.5, 0.5);
        const QRect drawRect{0, 0, rect().width() * 2, rect().height() * 2};
        painter.drawRoundedRect(drawRect.marginsRemoved({5, 5, 5, 5}), 5.0, 5.0);
    }
}

void CardWidget::actionEvent(QActionEvent *event)
{
    QApplication::sendEvent(&m_toolbar, event);
}

QLabel *CardWidget::title()
{
    return &m_title;
}

QVBoxLayout *CardWidget::innerLayout()
{
    return m_content;
}

QToolBar *CardWidget::toolbar()
{
    return &m_toolbar;
}

void CardWidget::setMiddleHidden(const bool hidden)
{
    m_middleWidget.setHidden(!hidden);
}

CardWidget::HighlightMode CardWidget::highlightMode() const
{
    return m_highlightMode;
}

void CardWidget::setHighlightMode(HighlightMode highlightMode)
{
    if (m_highlightMode != highlightMode) {
        m_highlightMode = highlightMode;
        update();
    }
}

CardWidget::CardWidget(QWidget *parent)
    : QPushButton(parent)
    , m_outer{new QVBoxLayout}
    , m_content{new QVBoxLayout}
{
    setObjectName("CardWidget");

    setLayout(m_outer);
    m_outer->setSpacing(Units::smallSpacing);

    m_title.setFont(Units::heading(m_title.font(), 1));
    m_title.setContentsMargins(Units::smallSpacing, Units::smallSpacing, Units::smallSpacing, 0);
    m_outer->addWidget(&m_title);

    m_middleWidget.setContentsMargins(Units::smallSpacing, 0, Units::smallSpacing, 0);
    m_content->setContentsMargins(0, 0, 0, 0);
    m_middleWidget.setLayout(m_content);

    m_outer->addWidget(&m_middleWidget, 1);

    const auto toolbarContainer = new QWidget();
    const auto toolbarLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    toolbarContainer->setLayout(toolbarLayout);

    toolbarLayout->addWidget(&m_toolbar, 1, Qt::AlignLeft);

    m_toolbar.setToolButtonStyle(Qt::ToolButtonFollowStyle);

    m_outer->addWidget(toolbarContainer);

    setStyleSheet("CardWidget { background: palette(base); }");
}

CardWidget::~CardWidget() = default;
