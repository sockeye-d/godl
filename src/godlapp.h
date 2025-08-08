#ifndef GODLAPP_H
#define GODLAPP_H

#include <KirigamiAddonsStatefulApp/AbstractKirigamiApplication>

class GodlApp : public AbstractKirigamiApplication
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit GodlApp(QObject *parent = nullptr);

    void setupActions() override;

    Q_SIGNAL void nextTabTriggered();
    Q_SIGNAL void prevTabTriggered();
};

#endif // GODLAPP_H
