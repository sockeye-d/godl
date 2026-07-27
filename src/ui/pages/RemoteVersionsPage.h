#ifndef REMOTEVERSIONSPAGE_H
#define REMOTEVERSIONSPAGE_H
#include "ui/page.h"

class RemoteVersionsPage : public Page
{
public:
    explicit RemoteVersionsPage(QWidget *parent);
    ~RemoteVersionsPage() override = default;

    QString name() override;
    QString iconName() override;
};

#endif //REMOTEVERSIONSPAGE_H
