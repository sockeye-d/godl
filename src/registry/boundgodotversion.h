#ifndef BOUNDVERSION_H
#define BOUNDVERSION_H
#include <QObject>
#include "serializable.h"
#include <qqmlintegration.h>

/**
 * @brief The BoundGodotVersion class is a simpler version of a @ref GodotVersion that only includes information needed to be bound to a project.
 */
struct BoundGodotVersion : public QObject, Serializable
{
    Q_OBJECT
private:
    Q_PROPERTY(QString tagName READ tagName WRITE setTagName NOTIFY tagNameChanged FINAL)
    QString m_tagName = "";

public:
    void setTagName(QString tagName)
    {
        if (m_tagName == tagName)
            return;
        m_tagName = tagName;
        Q_EMIT tagNameChanged();
    }

    QString tagName() const { return m_tagName; }

    Q_SIGNAL void tagNameChanged();

private:
    Q_PROPERTY(bool isMono READ isMono WRITE setIsMono NOTIFY isMonoChanged FINAL)
    bool m_isMono = "";

public:
    void setIsMono(bool isMono)
    {
        if (m_isMono == isMono)
            return;
        m_isMono = isMono;
        Q_EMIT isMonoChanged();
    }

    bool isMono() const { return m_isMono; }

    Q_SIGNAL void isMonoChanged();

    // Serializable interface
public:
    explicit BoundGodotVersion(QObject *parent = nullptr)
        : QObject(parent)
    {}
    void serialize(KConfigGroup config) override;
    void deserialize(KConfigGroup config) override;
};

#endif // BOUNDVERSION_H
