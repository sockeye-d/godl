#pragma once

#include <KXmlGuiWindow>
#include <QObject>

class QStackedLayout;
class Page;

class GodlWindow : public QMainWindow
{
    Q_OBJECT

    QList<Page *> pages{};
    QList<QAction *> builtinActions{};
    QList<QAction *> pageActions{};
    KToolBar *toolbar = nullptr;
    QStackedLayout *m_layout;

public:
    void activatePage(Page *page);

    explicit GodlWindow(QWidget *parent = nullptr);
    // ~GodlWindow() override;
};
