#pragma once

#include <KXmlGuiWindow>
#include <QObject>

class Page;

class GodlWindow : public KXmlGuiWindow
{
    Q_OBJECT

    QList<Page *> pages{};
    QList<QAction *> builtinActions{};
    QList<QAction *> pageActions{};
    KToolBar *toolbar = nullptr;

public:
    void activatePage(Page *page);

    explicit GodlWindow(QWidget *parent = nullptr);
    ~GodlWindow() override;
};
