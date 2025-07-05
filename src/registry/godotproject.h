#ifndef GODOTPROJECT_H
#define GODOTPROJECT_H

#include <QDateTime>
#include <QObject>
#include "boundgodotversion.h"
#include "serializable.h"
#include <KConfigGroup>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

class ProjectsRegistry;

class GodotProject : public QObject, Serializable
{
    friend class ProjectsRegistry;
    Q_OBJECT

public:
    enum OpenError {
        NoError,
        NoEditorBound,
        NoEditorFound,
        FailedToStartEditor,
    };
    Q_ENUM(OpenError)
private:
    ProjectsRegistry *m_registry = nullptr;
    KConfig *m_config = nullptr;

    void setConfig(KConfig *config)
    {
        m_config = config;
        save();
    }
    KConfig *config() { return m_config; }

private:
    Q_PROPERTY(BoundGodotVersion *godotVersion READ godotVersion WRITE setGodotVersion NOTIFY
                   godotVersionChanged FINAL)
    BoundGodotVersion *m_godotVersion = nullptr;

public:
    void setGodotVersion(BoundGodotVersion *godotVersion)
    {
        if (m_godotVersion == godotVersion)
            return;
        if (m_godotVersion)
            m_godotVersion->deleteLater();
        m_godotVersion = godotVersion;
        if (m_godotVersion)
            m_godotVersion->setParent(this);
        Q_EMIT godotVersionChanged();
        save();
    }

    BoundGodotVersion *godotVersion() const { return m_godotVersion; }

    Q_SIGNAL void godotVersionChanged();

private:
    Q_PROPERTY(bool favorite READ favorite WRITE setFavorite NOTIFY favoriteChanged FINAL)
    bool m_favorite = false;

public:
    void setFavorite(bool favorite);

    bool favorite() const;

    Q_SIGNAL void favoriteChanged();

private:
    Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged FINAL)
    QStringList m_tags = {};

public:
    void setTags(QStringList tags)
    {
        if (m_tags == tags)
            return;
        m_tags = tags;
        Q_EMIT tagsChanged();
        save();
    }

    QStringList tags() const { return m_tags; }

    Q_SIGNAL void tagsChanged();

private:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    QString m_name = "";

public:
    void setName(QString name)
    {
        if (m_name == name)
            return;
        m_name = name;
        Q_EMIT nameChanged();
        save();
    }

    QString name() const { return m_name; }

    Q_SIGNAL void nameChanged();

private:
    Q_PROPERTY(
        QString description READ description WRITE setDescription NOTIFY descriptionChanged FINAL)
    QString m_description = "";

public:
    void setDescription(QString description)
    {
        if (m_description == description)
            return;
        m_description = description;
        Q_EMIT descriptionChanged();
        save();
    }

    QString description() const { return m_description; }

    Q_SIGNAL void descriptionChanged();

private:
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged FINAL)
    QString m_path = "";

public:
    void setPath(QString path)
    {
        if (m_path == path)
            return;
        m_path = path;
        Q_EMIT pathChanged();
        Q_EMIT projectPathChanged();
    }

    QString path() const { return m_path; }

    Q_SIGNAL void pathChanged();

private:
    Q_PROPERTY(QDateTime lastEditedTime READ lastEditedTime WRITE setLastEditedTime NOTIFY
                   lastEditedTimeChanged FINAL)
    QDateTime m_lastEditedTime;

private:
    Q_PROPERTY(QString projectPath READ projectPath NOTIFY projectPathChanged FINAL)

public:
    QString projectPath() const;

    Q_SIGNAL void projectPathChanged();

public:
    void setLastEditedTime(QDateTime lastEditedTime)
    {
        if (m_lastEditedTime == lastEditedTime)
            return;
        m_lastEditedTime = lastEditedTime;
        Q_EMIT lastEditedTimeChanged();
    }

    QDateTime lastEditedTime() const { return m_lastEditedTime; }

    Q_SIGNAL void lastEditedTimeChanged();

public:
    explicit GodotProject(QObject *parent = nullptr);

    Q_INVOKABLE bool showInFolder() const;

    // Serializable interface
public:
    void serialize(KConfigGroup config) override;
    void deserialize(KConfigGroup config) override { deserialize(config, ""); }
    void deserialize(KConfigGroup config, QString path);

public:
    static inline const QString projectFilename = "godlproject";
    static GodotProject *load(const QString &path);
    Q_INVOKABLE void save();
    Q_INVOKABLE OpenError open() const;
};

#endif // GODOTPROJECT_H
