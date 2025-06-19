#ifndef GODLAPP_H
#define GODLAPP_H

#include <AbstractKirigamiApplication>

class GodlApp : public AbstractKirigamiApplication
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit GodlApp(QObject *parent = nullptr)
        : AbstractKirigamiApplication(parent)
    {
        AbstractKirigamiApplication::setupActions();
    }
};

#endif // GODLAPP_H
