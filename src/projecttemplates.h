#ifndef PROJECTTEMPLATES_H
#define PROJECTTEMPLATES_H

#include <QMap>
#include <QObject>
#include "template.h"
#include <qqmlintegration.h>

class ProjectTemplates : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList templates READ templateNames NOTIFY templatesChanged FINAL)
    QMap<QString, Template> m_templates{};

    void setTemplates(const QList<Template> &templates)
    {
        if (m_templates.values() == templates)
            return;
        m_templates.clear();
        for (const auto &t : templates) {
            m_templates[t.name] = t;
        }
        Q_EMIT templatesChanged();
    }

public:
    QList<Template> templates() const { return m_templates.values(); }
    QStringList templateNames() const { return m_templates.keys(); }
    Q_INVOKABLE Template templ(const QString &name) { return m_templates[name]; }

    Q_SIGNAL void templatesChanged();

public:
    static ProjectTemplates *instance()
    {
        static auto *instance = new ProjectTemplates();
        return instance;
    }

    explicit ProjectTemplates(QObject *parent = nullptr);
    Q_INVOKABLE void rescan();
    void extractDefault();
    Q_INVOKABLE void createProject(const QString &name,
                                   const QString &dest,
                                   const QVariant &replacements);
    Q_INVOKABLE bool isProjectValid(const QString &path) const;
    Q_INVOKABLE QString generateRandomName() const;
};

#endif // PROJECTTEMPLATES_H
