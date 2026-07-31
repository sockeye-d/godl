#include "downloadspopup.h"

#include <QListView>
#include <QPainter>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollBar>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>

#include "carddelegate.h"
#include "cardwidget.h"
#include "downloadmanager.h"
#include "downloadmanagermodel.h"
#include "sizeconverter.h"
#include "ui/style.h"
#include "units.h"

namespace {
class PositionEventFilter : public QObject
{
    Q_OBJECT

    DownloadsPopup *m_popup;

public:
    bool eventFilter(QObject *watched, QEvent *event) override;

    explicit PositionEventFilter(DownloadsPopup *popup)
        : QObject(popup)
        , m_popup{popup}
    {}
};

bool PositionEventFilter::eventFilter(QObject *, QEvent *event)
{
    switch (event->type()) {
    case QEvent::Move:
    case QEvent::Resize:
    case QEvent::Paint: {
        m_popup->updatePosition();
    } break;
    default:
        break;
    }

    return false;
}

class DownloadsDelegate : public CardDelegate
{
    Q_OBJECT

public:
    explicit DownloadsDelegate(QAbstractItemView *itemView, QObject *parent = nullptr)
        : CardDelegate(itemView, parent)
    {
        m_size = {0, Units::gridUnit * 7};
    }

protected:
    void setupCard(CardWidget *card, const QModelIndex &weakIndex) const override
    {
        const auto cancelAction = new QAction(QIcon::fromTheme("process-stop"), "");
        card->addAction(cancelAction);

        const auto wrapper = new QWidget();
        const auto wrapperLayout = new QHBoxLayout(wrapper);
        wrapperLayout->setContentsMargins(0, 0, 0, 0);

        const auto dlSpeedLabel = new QLabel();
        dlSpeedLabel->setMinimumWidth(Units::gridUnit * 3);
        wrapperLayout->addWidget(dlSpeedLabel);

        const auto dlProgressBar = new QProgressBar();
        dlProgressBar->setRange(0, 100);
        wrapperLayout->addWidget(dlProgressBar, 1);

        const auto dlProgressLabel = new QLabel();
        dlProgressLabel->setMinimumWidth(Units::gridUnit * 3);
        wrapperLayout->addWidget(dlProgressLabel);

        card->innerLayout()->addWidget(wrapper);

        connect(cancelAction, &QAction::triggered, this, [index = QPersistentModelIndex{weakIndex}] {
            const auto data = getDataPtrFrom<DownloadManagerModel>(index);
            DownloadManager::instance()->cancel(data->id());
        });
    }

    void updateCard(CardWidget *card, const QPersistentModelIndex &index) const override
    {
        using namespace SizeConverter;
        const auto data = getDataPtrFrom<DownloadManagerModel>(index);
        card->title()->setText(data->assetName());

        const auto innerInnerLayout = card->innerLayout()->itemAt(0)->widget()->layout();
        const auto dlSpeedLabel = qobject_cast<QLabel *>(innerInnerLayout->itemAt(0)->widget());
        const auto dlProgressBar = qobject_cast<QProgressBar *>(innerInnerLayout->itemAt(1)->widget());
        const auto dlProgressLabel = qobject_cast<QLabel *>(innerInnerLayout->itemAt(2)->widget());

        switch (data->stage()) {
        case DownloadInfo::Downloading: {
            card->actions().first()->setIcon(QIcon::fromTheme("process-stop"));
            card->setHighlightMode(CardWidget::HighlightNone);
            dlSpeedLabel->show();
            dlProgressBar->show();
            dlProgressLabel->show();

            dlSpeedLabel->setText(QStringLiteral("%1/s").arg(formatSize(static_cast<qint64>(data->downloadSpeed()))));
            dlProgressBar->setRange(0, static_cast<int>(data->total()));
            dlProgressBar->setValue(static_cast<int>(data->received()));
            dlProgressLabel->setText(QStringLiteral("%1/%2").arg(formatSize(data->received(), 1), formatSize(data->total(), 1)));
        } break;
        case DownloadInfo::Unzipping: {
            card->actions().first()->setIcon(QIcon::fromTheme("process-stop"));
            card->setHighlightMode(CardWidget::HighlightNone);
            dlSpeedLabel->show();
            dlProgressBar->show();
            dlProgressLabel->hide();

            dlSpeedLabel->setText(QStringLiteral("Extracting"));
            dlProgressBar->setValue(0);
            dlProgressBar->setRange(0, 0);
        } break;
        case DownloadInfo::Finished: {
            card->actions().first()->setIcon(QIcon::fromTheme("tab-close"));
            card->setHighlightMode(CardWidget::HighlightPositive);
            dlSpeedLabel->show();
            dlProgressBar->hide();
            dlProgressLabel->hide();

            dlSpeedLabel->setText(QStringLiteral("Done!"));
        } break;
        case DownloadInfo::UnzipError:
        case DownloadInfo::UnknownError:
        case DownloadInfo::DownloadError: {
            card->actions().first()->setIcon(QIcon::fromTheme("tab-close"));
            card->setHighlightMode(CardWidget::HighlightNegative);
            dlSpeedLabel->show();
            dlProgressBar->hide();
            dlProgressLabel->hide();

            dlSpeedLabel->setText(data->error());
        } break;
        }
    }
};
} // namespace

void DownloadsPopup::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

qreal DownloadsPopup::animationProgress() const
{
    return m_animationProgress;
}

void DownloadsPopup::updatePosition()
{
    if (m_animationProgress < 0.0001) {
        if (isVisible()) {
            hide();
            m_popupButton->setShortcut(QKeySequence());
        }
    } else {
        if (!isVisible()) {
            show();
            raise();
            m_popupButton->setShortcut(QKeySequence::Cancel);
        }
    }

    const auto buttonTR = parentWidget()->mapFromGlobal(m_popupButton->mapToGlobal(m_popupButton->rect().topRight()));
    const int height = static_cast<int>(500 * m_animationProgress);
    setGeometry({buttonTR.x() - width(), buttonTR.y() - height, 400, height});
}

void DownloadsPopup::setAnimationProgress(const qreal progress)
{
    m_animationProgress = progress;

    updatePosition();
}

void DownloadsPopup::popup()
{
    onPopupButtonClicked(true);
}

QGridLayout *DownloadsPopup::layout() const
{
    return m_layout;
}

void DownloadsPopup::onPopupButtonClicked(const bool checked)
{
    const auto animation = new QPropertyAnimation(this, "animationProgress", parent());
    animation->setDuration(Units::shortDuration);
    animation->setEndValue(checked ? 1 : 0);
    animation->setEasingCurve(QEasingCurve::OutExpo);
    animation->start();
}

DownloadsPopup::DownloadsPopup(QPushButton *popupButton, QWidget *parent)
    : QWidget(parent)
    , m_popupButton{popupButton}
{
    m_popupButton->installEventFilter(new PositionEventFilter(this));
    setMinimumSize(400, 0);

    m_layout = new QGridLayout();
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);
    const auto view = new QListView();
    m_layout->addWidget(view, 0, 0, 1, 1);

    view->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    view->setAutoFillBackground(false);
    view->setSpacing(Units::smallSpacing);
    view->verticalScrollBar()->setSingleStep(20);
    view->setStyleSheet(Style::listViewStylesheet());

    view->setItemDelegate(new DownloadsDelegate(view));
    view->setModel(DownloadManager::instance()->model());

    connect(m_popupButton, &QPushButton::clicked, this, &DownloadsPopup::onPopupButtonClicked);
}

#if __has_include("downloadspopup.moc")
#include "downloadspopup.moc"
#endif