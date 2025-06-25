#ifndef GODOTVERSION_H
#define GODOTVERSION_H

#include <QObject>
#include <QQmlEngine>

class GodotVersion : public QObject
{
    Q_OBJECT
    QML_ELEMENT
private:
    Q_PROPERTY(bool isMono READ isMono NOTIFY isMonoChanged FINAL)
    bool m_isMono = false;

    void setIsMono(bool isMono)
    {
        if (m_isMono == isMono)
            return;
        m_isMono = isMono;
        Q_EMIT isMonoChanged();
    }

public:
    bool isMono() const { return m_isMono; }

    Q_SIGNAL void isMonoChanged();

private:
    Q_PROPERTY(QString assetName READ assetName WRITE setAssetName NOTIFY assetNameChanged FINAL)
    QString m_assetName = "";

public:
    void setAssetName(QString assetName)
    {
        if (m_assetName == assetName)
            return;
        m_assetName = assetName;
        Q_EMIT assetNameChanged();
    }

    QString assetName() const { return m_assetName; }

    Q_SIGNAL void assetNameChanged();

private:
    Q_PROPERTY(QString sourceUrl READ sourceUrl NOTIFY sourceUrlChanged FINAL)
    QString m_sourceUrl = "";

    void setSourceUrl(QString sourceUrl)
    {
        if (m_sourceUrl == sourceUrl)
            return;
        m_sourceUrl = sourceUrl;
        Q_EMIT sourceUrlChanged();
    }

public:
    QString sourceUrl() const { return m_sourceUrl; }

    Q_SIGNAL void sourceUrlChanged();

public:
    explicit GodotVersion(QObject *parent = nullptr);
};

bool operator==(const GodotVersion &left, const GodotVersion &right);

#endif // GODOTVERSION_H
