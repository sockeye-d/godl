#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H
#include <QLineEdit>

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT

    QTimer *m_debounceTimer;
    bool m_delaySearch = false;

    void onSearchTriggered();
    void onTextChanged() const;
    void onTimerTriggered();

public:
    [[nodiscard]] bool delaySearch() const;
    void setDelaySearch(bool delaySearch);

    explicit SearchLineEdit(QWidget *parent = nullptr);
    explicit SearchLineEdit(bool delaySearch, QWidget *parent = nullptr);

    // ReSharper disable once CppFunctionIsNotImplemented
    Q_SIGNAL void searchTriggered(const QString &query);
};

#endif //SEARCHLINEEDIT_H
