#include "filelineedit.h"

#include <QFileDialog>
#include <QLineEdit>

#include <QHBoxLayout>
#include <QToolButton>

QLineEdit *FileLineEdit::lineEdit() const
{
    return m_lineEdit;
}

QFileDialog *FileLineEdit::fileDialog() const
{
    return m_fileDialog;
}

FileLineEdit::FileLineEdit(QWidget *parent)
    : QWidget(parent)
    , m_lineEdit{new QLineEdit()}
    , m_fileDialog{new QFileDialog(this)}
{
    const auto layout = new QHBoxLayout();
    setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(m_lineEdit, 1);

    const auto button = new QToolButton();
    button->setIcon(QIcon::fromTheme("document-open"));

    m_fileDialog->setModal(true);
    connect(button, &QToolButton::clicked, this, [this] {
        m_fileDialog->selectFile(m_lineEdit->text());
        m_fileDialog->show();
    });
    layout->addWidget(button);

    connect(m_fileDialog, &QFileDialog::accepted, this, [this]() {
        m_lineEdit->setText(m_fileDialog->selectedFiles().first());
    });
}

FileLineEdit::~FileLineEdit() = default;
