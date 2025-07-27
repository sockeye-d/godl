#ifndef GODLAPP_H
#define GODLAPP_H

#include <KirigamiAddonsStatefulApp/AbstractKirigamiApplication>

class GodlApp : public AbstractKirigamiApplication
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit GodlApp(QObject *parent = nullptr)
        : AbstractKirigamiApplication(parent)
    {
        GodlApp::setupActions();
    }

    void setupActions() override;
};

#endif // GODLAPP_H
