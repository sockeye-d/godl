#ifndef CONFIGSIGNALS_H
#define CONFIGSIGNALS_H

#include <QMetaObject>
#include <QMetaProperty>
#include <config.h>

class ConfigSignals : public QObject
{
    Q_OBJECT
    Config *const m_config;
    bool m_isSaveNeeded;

    Q_PROPERTY(bool isSaveNeeded READ isSaveNeeded NOTIFY isSaveNeededChanged FINAL)

    void setIsSaveNeeded(bool isSaveNeeded)
    {
        m_isSaveNeeded = isSaveNeeded;
        Q_EMIT isSaveNeededChanged();
    }

public:
    Q_INVOKABLE void onNotifySignal()
    {
        setIsSaveNeeded(m_config->isSaveNeeded());
        Q_EMIT configChanged();
    }

    Q_INVOKABLE void save()
    {
        m_config->save();
        setIsSaveNeeded(false);
    }

    explicit ConfigSignals(Config *config)
        : m_config(config)
    {
        auto meta = config->metaObject();
        // die stupid warning
        auto onNotifySignal = metaObject()->method(metaObject()->indexOfMethod("onNotifySignal()"));
        for (int i = 0; i < meta->propertyCount(); i++) {
            auto prop = meta->property(i);
            if (prop.hasNotifySignal()) {
                connect(config, prop.notifySignal(), this, onNotifySignal);
            }
        }
    }

    bool isSaveNeeded() const { return m_isSaveNeeded; }
    Q_SIGNAL void configChanged();
    Q_SIGNAL void isSaveNeededChanged();
};

#endif // CONFIGSIGNALS_H
