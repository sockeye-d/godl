#pragma once

#include <KXmlGuiWindow>
#include <QObject>

#include <KToolBar>

class QStackedLayout;
class Page;

class GodlWindow : public QMainWindow
{
    Q_OBJECT

    QList<Page *> pages{};
    QList<QAction *> builtinActions{};
    QList<QAction *> pageActions{};
    KToolBar toolbar;
    QStackedLayout *m_layout;

public:
    void activatePage(Page *page);

    explicit GodlWindow(QWidget *parent = nullptr);
    // ~GodlWindow() override;
};
