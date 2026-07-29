#include "RemoteVersionsPage.h"

#include <QListView>
#include <QScrollBar>

#include "ui/widgets/cardwidget.h"
#include "units.h"
#include "versionregistry.h"
#include <QCheckBox>
#include <QDateTime>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFormLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextEdit>

#include "chainedjsonrequest.h"
#include "ui/widgets/carddelegate.h"
#include <QNetworkRequest>
#include <QPointer>

#include "dateconverter.h"
#include "macros.h"
#include "network.h"

namespace {
struct LinkHeaderInfo
{
    QUrl link;
    QMap<QString, QString> data;
};

QList<LinkHeaderInfo> parseLinkHeader(const QByteArray &header)
{
    QList<LinkHeaderInfo> links;
    for (const auto &link : header.split(',')) {
        const auto segmentsView = link.split(';') | std::views::transform([](const QByteArray &ba) {
                                      return ba.trimmed();
                                  });
        const auto segments = QList(segmentsView.begin(), segmentsView.end());
        auto first = QString(segments.constFirst());
        first = removePrefix(first, "<");
        first = removeSuffix(first, ">");
        QMap<QString, QString> linkData;
        for (const auto &data : segments.sliced(1)) {
            auto dataSplit = data.split('=');
            const auto key = QString(dataSplit.constFirst());
            auto value = QString(dataSplit.constLast());
            value = removePrefix(value, "\"");
            value = removeSuffix(value, "\"");
            linkData[key] = value;
        }
        links.append({.link = QUrl(first), .data = linkData});
    }
    return links;
}

class VersionListModel : public AbstractSimpleListModel<QJsonValue>
{
    Q_OBJECT

    QPointer<QNetworkReply> m_running;
    QUrl m_nextUrl;

public:
    explicit VersionListModel(QObject *parent = nullptr)
        : AbstractSimpleListModel(parent) {};

    void fetchFrom(const QUrl &url = QUrl())
    {
        if (!m_running.isNull()) {
            m_running->abort();
        }

        QNetworkRequest req{url.isEmpty() ? m_nextUrl : url};
        AUTH(req);
        req.setRawHeader("X-GitHub-Api-Version"_ba, "2022-11-28"_ba);
        m_running = Network::manager().get(req);

        connect(m_running, &QNetworkReply::finished, this, [this] {
            const auto linkHeader = m_running->rawHeader("link");
            const auto links = parseLinkHeader(linkHeader);
            m_nextUrl = QUrl();
            for (const auto &[link, data] : links) {
                if (const auto rel = data.value("rel", ""); rel == "next") {
                    m_nextUrl = QUrl(link);
                    break;
                }
            }

            for (const QJsonValueRef value : QJsonDocument::fromJson(m_running->readAll()).array()) {
                append(value);
            }

            m_running->deleteLater();
        });
    }

    void fetchMore(const QModelIndex &) override { fetchFrom(); }

    [[nodiscard]] bool canFetchMore(const QModelIndex &) const override
    {
        return !m_nextUrl.isEmpty();
    }
};

class VersionCardDelegate : public CardDelegate
{
    Q_OBJECT

protected:
    void setupCard(CardWidget *card, const QModelIndex &index) const override
    {
        auto showPatchNotes = new QAction(QIcon::fromTheme("document-preview"), "Show patch notes");
        auto download = new QAction(QIcon::fromTheme("download"), "Download");
        auto open = new QAction(QIcon::fromTheme("link"), "Open");

        const auto label = new QLabel();
        card->innerLayout()->addWidget(label);

        card->addActions({showPatchNotes, download, open});
    }

    void updateCard(CardWidget *card, const QPersistentModelIndex &index) const override
    {
        const auto data = index.data(VersionListModel::DataRole).toJsonObject();
        card->title()->setText(data["tag_name"].toString());
        const auto label = qobject_cast<QLabel *>(card->innerLayout()->itemAt(0)->widget());
        label->setText(DateConverter::instance()->relativeFormat(
            QDateTime::fromString(data["created_at"].toString(), Qt::ISODate)));
    }

public:
    explicit VersionCardDelegate(QAbstractItemView *itemView, QObject *parent = nullptr)
        : CardDelegate(itemView, parent)
    {
        m_size = {0, Units::gridUnit * 6};
    }
};
} // namespace

RemoteVersionsPage::RemoteVersionsPage(QWidget *parent)
    : Page(parent)
{
    const auto layout = new QVBoxLayout(this);
    const auto view = new QListView(this);

    const auto model = new VersionListModel();
    view->setModel(model);
    model->fetchFrom(VersionRegistry::resolveSourceUrl("/godotengine/godot-builds")
                     + "/releases?per_page=100");

    view->setItemDelegate(new VersionCardDelegate(view));
    view->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    view->setAutoFillBackground(false);
    view->setSpacing(Units::smallSpacing);
    view->verticalScrollBar()->setSingleStep(20);

    layout->addWidget(view);
    layout->setContentsMargins(0, 0, 0, 0);
}

QString RemoteVersionsPage::name()
{
    return "Remote versions";
}

QString RemoteVersionsPage::iconName()
{
    return "globe";
}

#if __has_include("RemoteVersionsPage.moc")
#include "RemoteVersionsPage.moc"
#endif