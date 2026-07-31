#ifndef DOWNLOADSPOPUP_H
#define DOWNLOADSPOPUP_H
#include <QWidget>

class QPushButton;

class DownloadsPopup : public QWidget
{
    Q_OBJECT

    QPushButton *m_popupButton;
    qreal m_animationProgress = 0.0;

    void onPopupButtonClicked(bool checked);

protected:
    void paintEvent(QPaintEvent *event) override;

public:
    Q_PROPERTY(qreal animationProgress READ animationProgress WRITE setAnimationProgress);

    [[nodiscard]] qreal animationProgress() const;
    void updatePosition();
    void setAnimationProgress(qreal progress);
    void popup();

    explicit DownloadsPopup(QPushButton *popupButton, QWidget *parent = nullptr);
};

#endif //DOWNLOADSPOPUP_H
