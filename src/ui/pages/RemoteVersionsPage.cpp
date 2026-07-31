#include "RemoteVersionsPage.h"

#include <QListView>
#include <QScrollArea>
#include <QScrollBar>

#include "ui/widgets/cardwidget.h"
#include "units.h"
#include "versionregistry.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QMenu>
#include <QProgressBar>
#include <QStringListModel>
#include <QTextBrowser>
#include <QTextEdit>

#include "chainedjsonrequest.h"
#include "ui/widgets/carddelegate.h"
#include <QNetworkRequest>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QStackedLayout>
#include <QString>
#include <QToolButton>

#include "dateconverter.h"
#include "downloadmanager.h"
#include "macros.h"
#include "network.h"
#include "networkresponsecode.h"
#include "sizeconverter.h"
#include "ui/style.h"
#include "ui/widgets/searchlineedit.h"
#include "ui/widgets/timestamplabel.h"

using namespace Qt::StringLiterals;

namespace {
struct VersionJson
{
    struct AssetJson
    {
        QString name;
        QUrl downloadUrl;
        int size;
    };

    QString repo;
    QString tagName;
    QString patchNotes;
    QUrl pageUrl;
    QDateTime createdAt;
    QList<AssetJson> assets;
};

struct LinkHeaderInfo
{
    QUrl link;
    QMap<QString, QString> data;
};

QList<LinkHeaderInfo> parseLinkHeader(const QByteArray &header)
{
    QList<LinkHeaderInfo> links;
    for (const auto &link : header.split(',')) {
        const auto segmentsView = link.split(';') | std::views::transform([](const auto &ba) { return ba.trimmed(); });
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

QDateTime parseIsoDt(const QString &dt)
{
    return QDateTime::fromString(dt, Qt::ISODate);
}
}; // namespace

class VersionListModel : public AbstractSimpleListModel<VersionJson, true>
{
    Q_OBJECT

    QPointer<QNetworkReply> m_running;
    QUrl m_nextUrl;
    RemoteVersionsPage *m_page;

    int m_currentPageNumber = 0;
    int m_lastPageNumber = 0;

public:
    explicit VersionListModel(RemoteVersionsPage *page, QObject *parent = nullptr)
        : AbstractSimpleListModel(parent)
        , m_page{page} {};

    QList<VersionJson> parseData(const QByteArray &json) const
    {
        QList<VersionJson> data;
        for (const auto &value : QJsonDocument::fromJson(json).array()) {
            auto d = value.toObject();

            QList<VersionJson::AssetJson> assets{};
            for (const auto &asset : d[u"assets"_s].toArray()) {
                assets.append(VersionJson::AssetJson{.name = asset[u"name"_s].toString(),
                                                     .downloadUrl = QUrl(asset[u"browser_download_url"_s].toString()),
                                                     .size = asset[u"size"_s].toInt()});
            }

            data << VersionJson{.repo = m_page->m_currentRepo,
                                .tagName = d[u"tag_name"_s].toString(),
                                .patchNotes = d[u"body"_s].toString(),
                                .pageUrl = QUrl(d[u"html_url"_s].toString()),
                                .createdAt = parseIsoDt(d[u"created_at"_s].toString()),
                                .assets = assets};
        }
        return data;
    }

    void fetchFrom(const QUrl &url = QUrl())
    {
        if (!m_running.isNull()) {
            m_running->abort();
        }

        if (!url.isEmpty()) {
            clear();
            m_currentPageNumber = 0;
            m_page->setProgress(0, 0);
        }

        QNetworkRequest req{url.isEmpty() ? m_nextUrl : url};
        AUTH(req);
        req.setRawHeader("X-GitHub-Api-Version"_ba, "2022-11-28"_ba);
        m_running = Network::manager().get(req);

        connect(m_running, &QNetworkReply::finished, this, [this] {
            if (m_running->error() != QNetworkReply::NoError) {
                return;
            }

            const auto linkHeader = m_running->rawHeader(u"link"_s);
            const auto links = parseLinkHeader(linkHeader);
            m_nextUrl = QUrl();
            for (const auto &[link, data] : links) {
                if (const auto rel = data.value(u"rel"_s, u""_s); rel == u"next"_s) {
                    m_nextUrl = QUrl(link);
                }
                if (m_lastPageNumber == 0) {
                    if (const auto rel = data.value(u"rel"_s, u""_s); rel == u"last"_s) {
                        static QRegularExpression pageParser{u"(?:^|&)page=(.*)"_s};
                        m_lastPageNumber = pageParser.match(QUrl(link).query()).captured(1).toInt();
                    }
                }
            }

            m_page->setProgress(++m_currentPageNumber, m_lastPageNumber);

            appendRange(parseData(m_running->readAll()));

            m_running.clear();

            if (m_nextUrl.isEmpty()) {
                m_page->hideIndicators();
            } else {
                fetchFrom();
            }
        });

        auto onErrorOccurred = [this](const auto &) { m_page->setError(m_running->errorString()); };

        connect(m_running, &QNetworkReply::errorOccurred, this, onErrorOccurred);
    }
};

namespace {
class VersionListModelProxy : public QSortFilterProxyModel
{
    Q_OBJECT

protected:
    [[nodiscard]] bool filterAcceptsRow(const int source_row, const QModelIndex &source_parent) const override
    {
        const auto index = sourceModel()->index(source_row, 0, source_parent);
        const auto data = getDataFrom<VersionListModel>(index);
        return data->tagName.contains(filterRegularExpression());
    }

public:
    explicit VersionListModelProxy(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {}

    ~VersionListModelProxy() override = default;
};

QDialog *createPatchNoteDialog(const VersionJson *info, QWidget *parent)
{
    const auto dialog = new QDialog(parent, Qt::Dialog);
    dialog->setModal(true);

    const auto layout = new QVBoxLayout(dialog);

    using enum QDialogButtonBox::StandardButton;
    const auto buttons = new QDialogButtonBox(Close);

    const auto patchNotes = new QTextBrowser();
    const auto document = new QTextDocument();
    patchNotes->setOpenExternalLinks(true);
    patchNotes->setReadOnly(true);
    document->setMarkdown(info->patchNotes);
    patchNotes->setDocument(document);

    layout->addWidget(patchNotes);
    layout->addWidget(buttons);

    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::accept);

    dialog->resize(400, 300);
    dialog->setWindowTitle(u"Patch notes for %1"_s.arg(info->tagName));
    return dialog;
}

QDialog *createDownloadDialog(const QPersistentModelIndex &infoIndex, RemoteVersionsPage *parent)
{
    const auto dialog = new QDialog(parent, Qt::Dialog);
    const auto layout = new QVBoxLayout(dialog);
    dialog->setModal(true);

    using enum QDialogButtonBox::StandardButton;
    const auto buttons = new QDialogButtonBox(Close);

    const auto topBarWidget = new QWidget();
    layout->addWidget(topBarWidget);

    const auto topBar = new QHBoxLayout();
    topBar->setContentsMargins(0, 0, 0, 0);
    topBarWidget->setLayout(topBar);

    const auto filterLineEdit = new SearchLineEdit();
    filterLineEdit->setPlaceholderText("Filter...");
    topBar->addWidget(filterLineEdit, 1);

    const auto supportedOnly = new QPushButton();
    supportedOnly->setIcon(QIcon::fromTheme("view-filter"));
    supportedOnly->setText("Current platform only");
    supportedOnly->setCheckable(true);
    supportedOnly->setChecked(true);
    supportedOnly->setFlat(true);
    topBar->addWidget(supportedOnly);

    const auto monoOnly = new QPushButton();
    monoOnly->setIcon(QIcon::fromTheme("view-filter"));
    monoOnly->setText(".NET (Mono) versions only");
    monoOnly->setCheckable(true);
    monoOnly->setFlat(true);
    topBar->addWidget(monoOnly);

    const auto inner = new QScrollArea();
    layout->addWidget(inner);

    // the inner inner
    //
    // having trouble organizing your inners? well, look no further
    const auto innerInner = new QWidget();
    inner->setWidget(innerInner);
    inner->setWidgetResizable(true);
    inner->setAlignment(Qt::AlignTop);
    inner->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    innerInner->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    const auto cardLayout = new QVBoxLayout();
    cardLayout->setAlignment(Qt::AlignTop);
    innerInner->setLayout(cardLayout);

    const auto info = *getDataFrom<VersionListModel>(infoIndex);
    QList<std::pair<CardWidget *, VersionJson::AssetJson>> cards;
    for (const auto &asset : info.assets) {
        const auto card = new CardWidget();
        card->title()->setText(asset.name);
        card->setFixedHeight(Units::gridUnit * 6);

        if (VersionRegistry::instance()->downloadedAsset(info.tagName, info.repo, asset.name)) {
            card->setHighlightMode(CardWidget::HighlightPositive);
        }

        const auto label = new QLabel(asset.downloadUrl.toDisplayString());
        card->innerLayout()->addWidget(label);

        const auto downloadAction = new QAction(QIcon::fromTheme("download"),
                                                u"Download (%1)"_s.arg(SizeConverter::formatSize(asset.size, 0)));

        auto onDownloaded =
            [assetName = asset.name, assetDownloadUrl = asset.downloadUrl, tagName = info.tagName, repo = info.repo, parent]() {
                DownloadManager::instance()->download(assetName, tagName, assetDownloadUrl, repo);
                Q_EMIT parent->downloadStarted();
            };

        QObject::connect(downloadAction, &QAction::triggered, dialog, onDownloaded);

        card->addAction(downloadAction);
        cardLayout->addWidget(card);
        cards << std::pair{card, asset};
    }

    const auto updateFilters = [cards, filterLineEdit, supportedOnly, monoOnly] {
        for (const auto &[card, asset] : cards) {
            bool passesFilter = false;
            if (monoOnly->isChecked() && !asset.name.contains(u"mono"_s)) {
                goto fail;
            }
            if (!asset.name.contains(filterLineEdit->text())) {
                goto fail;
            }
            if (supportedOnly->isChecked()) {
                bool wasContained = false;
                for (const auto &filter : Config::downloadFilter()) {
                    if (asset.name.contains(filter)) {
                        wasContained = true;
                        break;
                    }
                }
                if (!wasContained) {
                    goto fail;
                }
            }
            passesFilter = true;

        fail:
            card->setVisible(passesFilter);
        }
    };

    QObject::connect(filterLineEdit, &SearchLineEdit::searchTriggered, dialog, updateFilters);
    QObject::connect(supportedOnly, &QPushButton::clicked, dialog, updateFilters);
    QObject::connect(monoOnly, &QPushButton::clicked, dialog, updateFilters);

    updateFilters();

    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::accept);

    dialog->resize(700, 500);
    dialog->setWindowTitle(u"Download %1"_s.arg(info.tagName));
    return dialog;
}
} // namespace

class VersionCardDelegate : public CardDelegate
{
    Q_OBJECT

protected:
    void setupCard(CardWidget *card, const QModelIndex &index) const override
    {
        auto showPatchNotes = new QAction(QIcon::fromTheme("document-preview"), "Show patch notes");
        auto download = new QAction(QIcon::fromTheme("download"), "Download");
        auto open = new QAction(QIcon::fromTheme("link"), "Open");

        const auto label = new TimestampLabel([](const auto &formatted) { return QStringLiteral("Created %1").arg(formatted); });

        card->innerLayout()->addWidget(label);

        card->addActions({showPatchNotes, download, open});

        QPersistentModelIndex modelIndex{index};

        connect(showPatchNotes, &QAction::triggered, card, [this, modelIndex] {
            const auto info = getDataFrom<VersionListModel>(modelIndex);
            const auto dialog = createPatchNoteDialog(info, itemView());
            dialog->open();
            connect(dialog, &QDialog::finished, dialog, [dialog](const int) { dialog->deleteLater(); });
        });

        connect(card, &CardWidget::clicked, card, [this, modelIndex] { Q_EMIT downloadRequested(modelIndex); });
        connect(download, &QAction::triggered, card, [this, modelIndex] { Q_EMIT downloadRequested(modelIndex); });

        connect(open, &QAction::triggered, card, [modelIndex] {
            const auto info = getDataFrom<VersionListModel>(modelIndex);
            QDesktopServices::openUrl(info->pageUrl);
        });

        connect(VersionRegistry::instance(), &VersionRegistry::hasVersionChanged, card, [card, index] {
            const auto data = getDataFrom<VersionListModel>(index);
            const bool isDownloaded = VersionRegistry::instance()->downloaded(data->tagName, data->repo);
            card->setHighlightMode(isDownloaded ? CardWidget::HighlightPositive : CardWidget::HighlightNone);
        });
    }

    void updateCard(CardWidget *card, const QPersistentModelIndex &index) const override
    {
        const auto data = getDataFrom<VersionListModel>(index);

        card->title()->setText(data->tagName);
        const auto label = qobject_cast<TimestampLabel *>(card->innerLayout()->itemAt(0)->widget());
        label->setTimestamp(data->createdAt);

        const bool isDownloaded = VersionRegistry::instance()->downloaded(data->tagName, data->repo);
        card->setHighlightMode(isDownloaded ? CardWidget::HighlightPositive : CardWidget::HighlightNone);
    }

public:
    explicit VersionCardDelegate(QAbstractItemView *itemView, QObject *parent = nullptr)
        : CardDelegate(itemView, parent)
    {
        m_size = {0, Units::gridUnit * 6};
    }

    Q_SIGNAL void downloadRequested(const QPersistentModelIndex &index) const;
};

QString RemoteVersionsPage::getUrlForRepo(const QString &repo)
{
    return VersionRegistry::resolveSourceUrl(repo) + "/releases?per_page=100";
}

RemoteVersionsPage::RemoteVersionsPage(QWidget *parent)
    : Page(parent)
    , m_errorIconLabel{new QLabel()}
    , m_errorLabel{new QLabel()}
    , m_bar{new QProgressBar()}
{
    const auto layout = new QVBoxLayout(this);
    const auto viewWrapper = new QWidget;
    const auto wrapperLayout = new QVBoxLayout(viewWrapper);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    wrapperLayout->setSpacing(0);
    const auto view = new QListView();
    view->setStyleSheet(Style::listViewStylesheet());

    wrapperLayout->addWidget(view, 1);

    m_versionListModel = new VersionListModel(this, this);
    m_sorter = new VersionListModelProxy();
    m_sorter->setSourceModel(m_versionListModel);
    view->setModel(m_sorter);

    m_delegate = new VersionCardDelegate(view);
    view->setItemDelegate(m_delegate);

    connect(m_delegate, &VersionCardDelegate::downloadRequested, this, [this](const QPersistentModelIndex &index) {
        const auto dialog = createDownloadDialog(index, this);
        connect(dialog, &QDialog::finished, dialog, [dialog] { dialog->deleteLater(); });
        dialog->open();
    });

    view->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    view->setAutoFillBackground(false);
    view->setSpacing(Units::smallSpacing);
    view->verticalScrollBar()->setSingleStep(20);

    layout->setContentsMargins(0, 0, 0, 0);

    m_barWrapper = new QWidget();
    const auto progressWrapperLayout = new QVBoxLayout(m_barWrapper);
    progressWrapperLayout->addWidget(m_bar);

    m_errorIconLabel->setPixmap(QIcon::fromTheme("network-disconnect").pixmap(128, 128));
    progressWrapperLayout->addWidget(m_errorIconLabel, 0, Qt::AlignHCenter);
    progressWrapperLayout->addWidget(m_errorLabel, 0, Qt::AlignHCenter);

    m_bar->setRange(0, 0);
    layout->addWidget(viewWrapper, 1);
    layout->addWidget(m_barWrapper, 0);

    hideIndicators();

    m_versionListModel->fetchFrom(getUrlForRepo(Config::sources().first()));
}

RemoteVersionsPage::~RemoteVersionsPage()
{
    delete m_delegate;
}

void RemoteVersionsPage::hideIndicators() const
{
    m_barWrapper->setVisible(false);
    m_bar->setVisible(false);
    m_errorLabel->setVisible(false);
    m_errorIconLabel->setVisible(false);
}

void RemoteVersionsPage::setProgress(const int currentPage, const int maxPages) const
{
    m_barWrapper->setVisible(true);
    m_bar->setVisible(true);
    m_errorLabel->setVisible(false);
    m_errorIconLabel->setVisible(false);
    if (maxPages > 0) {
        m_bar->setMaximum(maxPages);
    }
    m_bar->setValue(currentPage);
}

void RemoteVersionsPage::setError(const QString &errorText) const
{
    m_barWrapper->setVisible(true);
    m_bar->setVisible(false);
    m_errorLabel->setVisible(true);
    m_errorIconLabel->setVisible(true);
    m_errorLabel->setText(errorText);
}

QString RemoteVersionsPage::name()
{
    return u"Remote versions"_s;
}

QString RemoteVersionsPage::iconName()
{
    return u"globe"_s;
}

QList<std::variant<QAction *, QWidget *>> RemoteVersionsPage::actions()
{
    const auto filterLineEdit = new SearchLineEdit();
    filterLineEdit->setPlaceholderText("Filter (regex supported)");
    connect(filterLineEdit,
            &SearchLineEdit::searchTriggered,
            m_sorter,
            qOverload<const QString &>(&QSortFilterProxyModel::setFilterRegularExpression));
    filterLineEdit->setText(m_sorter->filterRegularExpression().pattern());

    const auto filterAction = new QToolButton();
    const auto menu = new QMenu();

    filterAction->setIcon(QIcon::fromTheme("filter-symbolic"));
    filterAction->setMenu(menu);

    connect(filterAction, &QToolButton::clicked, menu, [filterAction] { filterAction->showMenu(); });

    const auto makeFilterAction = [menu, filterLineEdit](const QString &name) {
        const auto action = new QAction(name);
        menu->addAction(action);
        connect(action, &QAction::triggered, [filterLineEdit, name] { filterLineEdit->setText(name); });
    };

    makeFilterAction("stable");
    makeFilterAction("rc");
    makeFilterAction("beta");
    makeFilterAction("alpha");
    makeFilterAction("dev");

    const auto repoComboBox = new QComboBox();
    repoComboBox->setEditable(true);
    const auto repoModel = new QStringListModel();
    const auto updateModel = [repoModel]() { repoModel->setStringList(Config::sources()); };
    connect(Config::self(), &Config::sourcesChanged, repoModel, updateModel);
    updateModel();

    repoComboBox->setModel(repoModel);

    if (m_currentRepo.isEmpty()) {
        m_currentRepo = repoComboBox->currentText();
    } else {
        repoComboBox->setCurrentText(m_currentRepo);
    }

    connect(repoComboBox, &QComboBox::textActivated, repoModel, [this](const QString &text) {
        m_currentRepo = text;
        m_versionListModel->fetchFrom(getUrlForRepo(text));
    });

    return {filterLineEdit, filterAction, repoComboBox};
}

#if __has_include("RemoteVersionsPage.moc")
#include "RemoteVersionsPage.moc"
#endif