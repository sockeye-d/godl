#ifndef CARDDELEGATE_H
#define CARDDELEGATE_H

#include "cardwidget.h"
#include <KWidgetItemDelegate>
#include <QObject>
#include <QStyledItemDelegate>

class CardDelegate : public KWidgetItemDelegate
{
    Q_OBJECT

protected:
    [[nodiscard]] QList<QWidget *> createItemWidgets(const QModelIndex &index) const override;
    void updateItemWidgets(const QList<QWidget *> &widgets,
                           const QStyleOptionViewItem &option,
                           const QPersistentModelIndex &index) const override;

public:
    explicit CardDelegate(QAbstractItemView *itemView, QObject *parent = nullptr)
        : KWidgetItemDelegate(itemView, parent)
    {}

    ~CardDelegate() override = default;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif //CARDDELEGATE_H
