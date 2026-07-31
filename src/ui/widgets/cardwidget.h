#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>

class CardWidget : public QPushButton
{
    Q_OBJECT

public:
    enum HighlightMode {
        HighlightNone,
        HighlightNegative,
        HighlightNeutral,
        HighlightPositive,
    };

    Q_ENUM(HighlightMode);

private:
    QVBoxLayout *m_outer;
    QLabel m_title;
    QVBoxLayout *m_content;
    QToolBar m_toolbar;
    QWidget m_middleWidget;

    HighlightMode m_highlightMode = HighlightNone;

protected:
    void paintEvent(QPaintEvent *event) override;
    void actionEvent(QActionEvent *event) override;

public:
    [[nodiscard]] QLabel *title();
    [[nodiscard]] QVBoxLayout *innerLayout();
    [[nodiscard]] QToolBar *toolbar();
    void setMiddleHidden(bool hidden);

    [[nodiscard]] HighlightMode highlightMode() const;
    void setHighlightMode(HighlightMode highlightMode);

    explicit CardWidget(QWidget *parent = nullptr);
    ~CardWidget() override;
};

#endif //CARDWIDGET_H
