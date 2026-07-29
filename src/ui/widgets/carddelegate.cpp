#include "carddelegate.h"

#include "cardwidget.h"
#include "versionregistrymodel.h"

QList<QWidget *> CardDelegate::createItemWidgets(const QModelIndex &index) const
{
    auto card = new CardWidget();
    setupCard(card, index);
    return {card};
}

void CardDelegate::updateItemWidgets(const QList<QWidget *> &widgets,
                                     const QStyleOptionViewItem &option,
                                     const QPersistentModelIndex &index) const
{
    const auto card = qobject_cast<CardWidget *>(widgets[0]);
    updateCard(card, index);
    card->setGeometry(QRect({0, 0}, option.rect.size()));
}

QSize CardDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    return m_size;
}

void CardDelegate::paint(QPainter *,
                         const QStyleOptionViewItem &,
                         const QModelIndex &) const
{}