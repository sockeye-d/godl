#ifndef PAGE_H
#define PAGE_H
#include <qwidget.h>

class Page : public QWidget
{
protected:
    explicit Page(QWidget *parent);

public:
    ~Page() override = default;

    virtual QString name() = 0;
    virtual QString iconName() = 0;

    virtual QList<QAction *> actions() { return {}; };
};

#endif //PAGE_H
