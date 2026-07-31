#include "searchlineedit.h"

#include <QTimer>

#include "units.h"

SearchLineEdit::SearchLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , m_debounceTimer{new QTimer(this)}
{
    const auto searchIcon = addAction(QIcon::fromTheme("search"), LeadingPosition);
    m_debounceTimer->setSingleShot(true);

    searchIcon->setShortcut(QKeySequence::Find);
    connect(searchIcon, &QAction::triggered, this, &SearchLineEdit::onSearchTriggered);

    connect(this, &QLineEdit::textChanged, this, &SearchLineEdit::onTextChanged);
    connect(this, &QLineEdit::editingFinished, this, &SearchLineEdit::onTimerTriggered);
    connect(m_debounceTimer, &QTimer::timeout, this, &SearchLineEdit::onTimerTriggered);
}

SearchLineEdit::SearchLineEdit(const bool delaySearch, QWidget *parent) : SearchLineEdit(parent)
{
    m_delaySearch = delaySearch;
}

void SearchLineEdit::onSearchTriggered()
{
    setFocus(Qt::ShortcutFocusReason);
}

void SearchLineEdit::onTextChanged() const
{
    m_debounceTimer->start(m_delaySearch ? Units::longDuration : Units::veryShortDuration);
}

void SearchLineEdit::onTimerTriggered()
{
    if (m_debounceTimer->isActive()) {
        m_debounceTimer->stop();
    }

    Q_EMIT searchTriggered(text());
}

bool SearchLineEdit::delaySearch() const
{
    return m_delaySearch;
}

void SearchLineEdit::setDelaySearch(const bool delaySearch)
{
    m_delaySearch = delaySearch;
}
