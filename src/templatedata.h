#ifndef TEMPLATEDATA_H
#define TEMPLATEDATA_H

#include <QList>
#include <QMap>
#include <QString>

struct TemplateHeader
{
    QString label = "";
};

struct TemplateString
{
    QString label = "";
    QString templ = "";
};

struct TemplateMultistring
{
    QString label = "";
    QString templ = "";
};

struct TemplateEnum
{
    struct Value
    {
        QString key = "";
        QString label = "";
    };

    QString label = "";
    QString templ = "";
    QList<Value> values = {};
};

using TemplateItem = std::variant<TemplateHeader, TemplateString, TemplateMultistring, TemplateEnum>;

struct TemplateData
{
    QMap<QString, TemplateItem> replacements;

    static TemplateData parse(const QString &meta);
};

#endif // TEMPLATEDATA_H
