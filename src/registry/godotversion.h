#ifndef GODOTVERSION_H
#define GODOTVERSION_H

#include <QObject>
#include <QQmlEngine>
#include "boundgodotversion.h"
#include <KSharedConfig>
#include <config.h>
#include <qtmetamacros.h>

class GodotVersion : public QObject
{
    friend class VersionRegistry;
    Q_OBJECT
    QML_ELEMENT

    QString m_groupName = "";

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

private:
    Q_PROPERTY(QString path READ path NOTIFY pathChanged FINAL)
    QString m_path = "";

    void setPath(QString path)
    {
        if (m_path == path)
            return;
        m_path = path;
        Q_EMIT pathChanged();
        Q_EMIT absolutePathChanged();
    }

public:
    QString path() const { return m_path; }

    Q_SIGNAL void pathChanged();

private:
    Q_PROPERTY(QString absolutePath READ absolutePath NOTIFY absolutePathChanged FINAL)
public:
    QString absolutePath() const { return Config::godotLocation() / path(); }

    Q_SIGNAL void absolutePathChanged();

private:
    Q_PROPERTY(QString tag READ tag NOTIFY tagChanged FINAL)
    QString m_tag = "";

    void setTag(QString tag)
    {
        if (m_tag == tag)
            return;
        m_tag = tag;
        Q_EMIT tagChanged();
    }

public:
    QString tag() const { return m_tag; }

    Q_SIGNAL void tagChanged();

private:
    Q_PROPERTY(QString repo READ repo NOTIFY repoChanged FINAL)
    QString m_repo = "";

    void setRepo(QString repo)
    {
        if (m_repo == repo)
            return;
        m_repo = repo;
        Q_EMIT repoChanged();
    }

public:
    QString repo() const { return m_repo; }

    Q_SIGNAL void repoChanged();

private:
    Q_PROPERTY(QString cmd READ cmd WRITE setCmd NOTIFY cmdChanged FINAL)
    // available replacements are
    // {executable}, {projectPath}
    QString m_cmd = Config::defaultCommand();

public:
    void setCmd(QString cmd)
    {
        if (m_cmd == cmd)
            return;
        m_cmd = cmd;
        Q_EMIT cmdChanged();
    }
    QString cmd() const { return m_cmd; }

    Q_SIGNAL void cmdChanged();

public:
    explicit GodotVersion(QObject *parent = nullptr)
        : GodotVersion("", "", "", "", "", false, parent)
    {}
    explicit GodotVersion(QString tag,
                          QString assetName,
                          QString sourceUrl,
                          QString repo,
                          QString path,
                          bool isMono,
                          QObject *parent = nullptr);

    void writeTo(KSharedConfig::Ptr config) const;
    Q_INVOKABLE void showExternally() const;
    Q_INVOKABLE void start(bool showOutput = true) const;
    Q_INVOKABLE BoundGodotVersion *boundVersion() const;

    Q_INVOKABLE QString toString() const;
    Q_INVOKABLE bool equals(const BoundGodotVersion *other)
    {
        if (!other) {
            return false;
        }
        return tag() == other->tagName() && isMono() == other->isMono();
    }
};

bool operator==(const GodotVersion &left, const GodotVersion &right);

QDebug operator<<(QDebug dbg, const GodotVersion &godotVersion);

#endif // GODOTVERSION_H
