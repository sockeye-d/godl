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

public:
    enum HighlightMode {
        HighlightNone,
        HighlightNegative,
        HighlightNeutral,
        HighlightPositive,
    };

    Q_ENUM(HighlightMode);

private:
    QLabel *m_title;
    QVBoxLayout *m_content;
    QToolBar *m_toolbar;
    QWidget *m_middleWidget;

    HighlightMode m_highlightMode = HighlightNone;

protected:
    void paintEvent(QPaintEvent *event) override;
    void actionEvent(QActionEvent *event) override;

public:
    [[nodiscard]] QLabel *title() const;
    [[nodiscard]] QVBoxLayout *innerLayout() const;
    [[nodiscard]] QToolBar *toolbar() const;
    void setMiddleHidden(bool hidden) const;

    [[nodiscard]] HighlightMode highlightMode() const;
    void setHighlightMode(HighlightMode highlightMode);

    explicit CardWidget(QWidget *parent = nullptr);
    ~CardWidget() override;
};

#endif //CARDWIDGET_H
