#ifndef CARDDELEGATE_H
#define CARDDELEGATE_H

class CardWidget;
#include <KWidgetItemDelegate>

class CardDelegate : public KWidgetItemDelegate
{
    Q_OBJECT

protected:
    QSize m_size;

    [[nodiscard]] QList<QWidget *> createItemWidgets(const QModelIndex &index) const override;
    void updateItemWidgets(const QList<QWidget *> &widgets,
                           const QStyleOptionViewItem &option,
                           const QPersistentModelIndex &index) const override;

    virtual void setupCard(CardWidget *card) const = 0;
    virtual void updateCard(CardWidget *card, const QPersistentModelIndex &index) const = 0;

    template<typename T>
    T *getData(const QPersistentModelIndex &index, const int role = Qt::UserRole + 1) const
        requires(std::convertible_to<T *, QObject *>)
    {
        return qSharedPointerFromVariant<T>(index.data(role)).data();
    }

public:
    explicit CardDelegate(QAbstractItemView *itemView, QObject *parent = nullptr)
        : KWidgetItemDelegate(itemView, parent)
    {}

    ~CardDelegate() override = default;

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif //CARDDELEGATE_H
