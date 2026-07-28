#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>

class QLabel;

class CardWidget : public QPushButton
{
    Q_OBJECT

    QLabel *m_title;
    QVBoxLayout *m_content;
    QToolBar *m_toolbar;

protected:
    void actionEvent(QActionEvent *event) override;

public:
    [[nodiscard]] QLabel *title() const;
    [[nodiscard]] QLayout *innerLayout() const;

    explicit CardWidget(QWidget *parent = nullptr);
    ~CardWidget() override;
};

#endif //CARDWIDGET_H
