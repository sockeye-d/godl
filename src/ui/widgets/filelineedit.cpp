#include "filelineedit.h"

#include <QFileDialog>
#include <QLineEdit>

#include <QHBoxLayout>
#include <QToolButton>

QLineEdit *FileLineEdit::lineEdit()
{
    return this;
}

QFileDialog *FileLineEdit::fileDialog() const
{
    return m_fileDialog;
}

FileLineEdit::FileLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , m_fileDialog{new QFileDialog(this)}
{
    const auto action = new QAction(QIcon::fromTheme("document-open"), "Open");
    addAction(action, TrailingPosition);

    m_fileDialog->setModal(true);
    connect(action, &QAction::triggered, this, [this] {
        m_fileDialog->selectFile(text());
        m_fileDialog->show();
    });

    connect(m_fileDialog, &QFileDialog::accepted, this, [this]() {
        setText(m_fileDialog->selectedFiles().first());
    });
}

FileLineEdit::~FileLineEdit() = default;
