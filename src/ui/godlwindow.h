#pragma once

#include <KXmlGuiWindow>
#include <QObject>

class Page;

class GodlWindow : public KXmlGuiWindow
{
    Q_OBJECT
    QList<Page *> pages;
public:
    explicit GodlWindow(QWidget *parent = nullptr);
    ~GodlWindow() override;
};
