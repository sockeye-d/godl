#include "LocalVersionsPage.h"

#include <QListView>
#include <QScrollBar>

#include "ui/widgets/cardwidget.h"
#include "units.h"
#include "versionregistry.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QToolButton>

#include "ui/widgets/carddelegate.h"
#include "ui/widgets/filelineedit.h"

namespace {
QDialog *createCommandEditWindow(GodotVersion *version, QWidget *parent)
{
    const auto monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    const auto dialog = new QDialog(parent, Qt::Dialog);
    const auto layout = new QVBoxLayout();
    dialog->setLayout(layout);
    dialog->setMinimumHeight(150);
    dialog->setWindowModality(Qt::ApplicationModal);
    const auto footer = new QWidget();
    const auto footerLayout = new QHBoxLayout();
    footerLayout->setContentsMargins(0, 0, 0, 0);
    footer->setLayout(footerLayout);
    footer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    const auto footerText = new QLabel(
        "<code>{executable}</code> expands to the absolute executable "
        "path<br><code>{projectPath}</code> expands to the absolute project.godot path");
    footerText->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    footerText->setMinimumWidth(footerText->sizeHint().width());
    footerLayout->addWidget(footerText, 1);

    using enum QDialogButtonBox::StandardButton;
    const auto buttons = new QDialogButtonBox(Ok | Cancel | Reset);
    footerLayout->addWidget(buttons);

    const auto content = new QTextEdit();
    content->setFont(monoFont);
    content->setText(version->cmd());

    auto onClicked = [buttons, content, dialog, version](QAbstractButton *button) {
        const auto btnType = buttons->standardButton(button);
        if (btnType == Reset) {
            content->setText(Config::defaultCommand());
            content->setFocus(Qt::OtherFocusReason);
        }

        if (btnType == Ok) {
            version->setCmd(content->toPlainText());
            dialog->accept();
        }

        if (btnType == Cancel) {
            dialog->reject();
        }
    };

    QObject::connect(buttons, &QDialogButtonBox::clicked, dialog, onClicked);

    layout->addWidget(content, 1);
    layout->addWidget(footer, 0);

    return dialog;
}

QDialog *createImportVersionWindow(QWidget *parent)
{
    const auto dialog = new QDialog(parent, Qt::Dialog);

    using enum QDialogButtonBox::StandardButton;
    const auto buttons = new QDialogButtonBox(Ok | Cancel);
    const auto layout = new QVBoxLayout();

    const auto formContainer = new QWidget();
    const auto form = new QFormLayout(formContainer);

    const auto location = new FileLineEdit();
    location->fileDialog()->setFileMode(QFileDialog::ExistingFile);

    form->addRow("Location:", location);

    const auto autodetect = new QToolButton();
    autodetect->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    autodetect->setIcon(QIcon::fromTheme("autocorrection"));
    autodetect->setText("Autodetect settings");
    form->addWidget(autodetect);

    const auto repository = new QLineEdit();
    form->addRow("Repository:", repository);

    const auto tag = new QLineEdit();
    form->addRow("Tag:", tag);

    const auto asset = new QLineEdit();
    form->addRow("Asset:", asset);

    const auto mono = new QCheckBox();
    form->addRow("Mono (C#):", mono);

    auto updateAutodetectable = [autodetect, location] {
        autodetect->setDisabled(
            !VersionRegistry::canAutodetect()
            || !VersionRegistry::canAutodetectFile(location->lineEdit()->text()));
    };

    QObject::connect(location->lineEdit(), &QLineEdit::textChanged, dialog, updateAutodetectable);
    updateAutodetectable();

    auto onAutodetect = [location, repository, tag, asset, mono] {
        const auto path = location->lineEdit()->text();
        repository->setText(VersionRegistry::instance()->detectRepository(path));
        tag->setText(VersionRegistry::instance()->detectTag(path));
        asset->setText(VersionRegistry::instance()->detectAsset(path));
        mono->setChecked(VersionRegistry::instance()->detectMono(path));
    };

    auto updateSubmittable = [buttons, location, repository, tag, asset] {
        bool disabled = true;
        if (location->lineEdit()->text().isEmpty())
            goto exit;
        if (repository->text().isEmpty())
            goto exit;
        if (tag->text().isEmpty())
            goto exit;
        if (asset->text().isEmpty())
            goto exit;
        disabled = false;
    exit:
        buttons->button(Ok)->setDisabled(disabled);
    };

    QObject::connect(autodetect, &QToolButton::pressed, dialog, onAutodetect);

    QObject::connect(location->lineEdit(), &QLineEdit::textChanged, dialog, updateSubmittable);
    QObject::connect(repository, &QLineEdit::textChanged, dialog, updateSubmittable);
    QObject::connect(tag, &QLineEdit::textChanged, dialog, updateSubmittable);
    QObject::connect(asset, &QLineEdit::textChanged, dialog, updateSubmittable);
    QObject::connect(mono, &QPushButton::toggled, dialog, updateSubmittable);

    auto onAccepted = [location, repository, tag, asset, mono, dialog] {
        VersionRegistry::instance()->registerLocalVersion(location->lineEdit()->text(),
                                                          repository->text(),
                                                          tag->text(),
                                                          asset->text(),
                                                          mono->isChecked());
        dialog->deleteLater();
    };

    QObject::connect(buttons, &QDialogButtonBox::accepted, dialog, onAccepted);

    dialog->setLayout(layout);
    dialog->setMinimumHeight(150);
    layout->addWidget(formContainer, 1);
    layout->addWidget(buttons);

    return dialog;
}

QDialog *createCleanOrphansDialog(QWidget *parent)
{
    const auto dialog = new QDialog(parent, Qt::Dialog);

    using enum QDialogButtonBox::StandardButton;
    const auto buttons = new QDialogButtonBox(Ok | Cancel);
    const auto layout = new QVBoxLayout(dialog);

    auto leakedBinaries = VersionRegistry::instance()->detectLeakedVersions();

    const auto inner = new QWidget();
    const auto innerLayout = new QVBoxLayout(inner);
    innerLayout->setAlignment(Qt::AlignVCenter);
    innerLayout->setContentsMargins(0, 0, 0, 0);
    QMap<QString, QCheckBox *> leakedBinaryCheckboxes;
    for (const auto &binary : leakedBinaries) {
        const auto checkbox = new QCheckBox(binary);
        leakedBinaryCheckboxes[binary] = checkbox;
        innerLayout->addWidget(checkbox);
    }

    if (leakedBinaries.isEmpty()) {
        const auto label = new QLabel();
        label->setPixmap(
            QIcon::fromTheme("edit-clear-all").pixmap(Units::gridUnit * 2, Units::gridUnit * 2));
        innerLayout->addWidget(label, 1, Qt::AlignCenter);
        innerLayout->addWidget(new QLabel("You're all clean!"), 1, Qt::AlignCenter);
    }

    layout->addWidget(inner, 1);

    auto onAccepted = [leakedBinaries, leakedBinaryCheckboxes, dialog] {
        auto toClear = leakedBinaries
                       | std::views::filter([leakedBinaryCheckboxes](const QString &path) {
                             return leakedBinaryCheckboxes.contains(path);
                         });
        VersionRegistry::deleteLeakedVersions(QStringList(toClear.begin(), toClear.end()));
        dialog->deleteLater();
    };

    QObject::connect(buttons, &QDialogButtonBox::accepted, dialog, onAccepted);
    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::deleteLater);

    layout->addWidget(buttons);
    dialog->setMinimumHeight(150);
    return dialog;
}

class VersionCardDelegate : public CardDelegate
{
    Q_OBJECT

protected:
    void setupCard(CardWidget *card, const QModelIndex &index) const override
    {
        auto showFolder = new QAction(QIcon::fromTheme("folder-open"), "Show folder");
        auto run = new QAction(QIcon::fromTheme("system-run"), "Run");
        auto editCommand = new QAction(QIcon::fromTheme("dialog-scripts"), "Edit command");
        auto remove = new QAction(QIcon::fromTheme("delete"), "Remove");

        card->addActions({showFolder, run, editCommand, remove});
        const auto label = new QLabel();
        label->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
        card->innerLayout()->addWidget(label);

        QPersistentModelIndex modelIndex{index};
        connect(showFolder, &QAction::triggered, card, [modelIndex, this] {
            getDataPtr<GodotVersion>(modelIndex)->showExternally();
        });
        connect(run, &QAction::triggered, card, [modelIndex, this] {
            getDataPtr<GodotVersion>(modelIndex)->start();
        });
        connect(editCommand, &QAction::triggered, card, [modelIndex, this] {
            const auto version = getDataPtr<GodotVersion>(modelIndex);
            const auto dialog = createCommandEditWindow(version, itemView());
            dialog->open();
            connect(dialog, &QDialog::finished, dialog, [modelIndex, dialog](const int) {
                dialog->deleteLater();
            });
        });
        connect(remove, &QAction::triggered, card, [modelIndex, this] {
            VersionRegistry::instance()->removeVersion(getDataPtr<GodotVersion>(modelIndex));
        });
        connect(card, &CardWidget::pressed, this, [modelIndex, this] {
            getDataPtr<GodotVersion>(modelIndex)->showExternally();
        });
    }

    void updateCard(CardWidget *card, const QPersistentModelIndex &index) const override
    {
        const auto version = getDataPtr<GodotVersion>(index);
        card->title()->setText(version->toString());
        const auto label = qobject_cast<QLabel *>(card->innerLayout()->itemAt(0)->widget());
        label->setText(version->cmd());
    }

public:
    explicit VersionCardDelegate(QAbstractItemView *itemView, QObject *parent = nullptr)
        : CardDelegate(itemView, parent)
    {
        m_size = {0, Units::gridUnit * 6};
    }
};
} // namespace

LocalVersionsPage::LocalVersionsPage(QWidget *parent)
    : Page(parent)
{
    const auto layout = new QVBoxLayout(this);
    const auto view = new QListView(this);

    view->setModel(VersionRegistry::instance()->model());
    view->setItemDelegate(new VersionCardDelegate(view));
    view->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    view->setAutoFillBackground(false);
    view->setSpacing(Units::smallSpacing);
    view->verticalScrollBar()->setSingleStep(20);

    layout->addWidget(view);
    layout->setContentsMargins(0, 0, 0, 0);
}

QString LocalVersionsPage::name()
{
    return "Local versions";
}

QString LocalVersionsPage::iconName()
{
    return "drive";
}

QList<QAction *> LocalVersionsPage::actions()
{
    auto manualVersion = new QAction(QIcon::fromTheme("document-import"), "Add version from file");
    auto cleanLeakedBinaries = new QAction(QIcon::fromTheme("edit-clear-all"),
                                           "Clean leaked binaries");

    connect(manualVersion, &QAction::triggered, this, [this] {
        const auto dialog = createImportVersionWindow(this);
        dialog->open();
        connect(dialog, &QDialog::finished, dialog, [dialog](const int) { dialog->deleteLater(); });
    });

    connect(cleanLeakedBinaries, &QAction::triggered, this, [this] {
        const auto dialog = createCleanOrphansDialog(this);
        dialog->open();
        connect(dialog, &QDialog::finished, dialog, [dialog](const int) { dialog->deleteLater(); });
    });

    return {manualVersion, cleanLeakedBinaries};
}

#if __has_include("LocalVersionsPage.moc")
#include "LocalVersionsPage.moc"
#endif