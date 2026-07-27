#ifndef LOCALVERSIONSPAGE_H
#define LOCALVERSIONSPAGE_H
#include "ui/page.h"

class LocalVersionsPage : public Page
{
public:
    explicit LocalVersionsPage(QWidget *parent);
    ~LocalVersionsPage() override = default;

    QString name() override;
    QString iconName() override;
};

#endif //LOCALVERSIONSPAGE_H
