#include "carddelegate.h"

#include "versionregistrymodel.h"

QList<QWidget *> CardDelegate::createItemWidgets(const QModelIndex &index) const
{
    QList<QAction *> actions;
    for (size_t i = 0; i < 10; i++) {
        const auto action = new QAction();
        action->setText(QString("Action %1").arg(i));
        action->setIcon(QIcon::fromTheme("document-edit"));
        actions << action;
    }
    const auto cardWidget = new CardWidget();
    cardWidget->addActions(actions);
    return {cardWidget};
}

void CardDelegate::updateItemWidgets(const QList<QWidget *> &widgets,
                                     const QStyleOptionViewItem &option,
                                     const QPersistentModelIndex &index) const
{
    const GodotVersion *version
        = qSharedPointerFromVariant<GodotVersion>(index.data(VersionRegistryModel::DataRole)).data();
    for (const auto &widget : widgets) {
        if (auto card = qobject_cast<CardWidget *>(widget)) {
            card->title()->setText(version->toString());
        }
        widget->setGeometry(QRect({0, 0}, option.rect.size()));
    }
}

QSize CardDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return {0, 200};
}

void CardDelegate::paint(QPainter *painter,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{}