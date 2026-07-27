#ifndef PROJECTPAGE_H
#define PROJECTPAGE_H
#include "ui/page.h"

class ProjectsPage : public Page
{
public:
    explicit ProjectsPage(QWidget *parent);
    ~ProjectsPage() override = default;

    QString name() override;
    QString iconName() override;
};

#endif //PROJECTPAGE_H
