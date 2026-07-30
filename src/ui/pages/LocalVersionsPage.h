#ifndef LOCALVERSIONSPAGE_H
#define LOCALVERSIONSPAGE_H
#include "ui/page.h"

class LocalVersionsPage : public Page
{
    Q_OBJECT
public:
    explicit LocalVersionsPage(QWidget *parent = nullptr);
    ~LocalVersionsPage() override = default;

    QString name() override;
    QString iconName() override;

    QList<std::variant<QAction *, QWidget *>> actions() override;
};

#endif //LOCALVERSIONSPAGE_H
