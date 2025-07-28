#include "templatedata.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#define forconst(name, iterator) \
    for (const auto &name##list = iterator; const auto &name : name##list)

TemplateData TemplateData::parse(const QString &meta)
{
    TemplateData data;
    const QJsonDocument json = QJsonDocument::fromJson(meta.toUtf8());

    for (const auto &repl : json.object().value("replacements").toArray()) {
        const QString &type = repl.toObject().value("type").toString();
        const QString &label = repl.toObject().value("label").toString();
        const QString &templStr = repl.toObject().value("template").toString();
        if (type == "string") {
            data.replacements[templStr] = TemplateItem(TemplateString{label, templStr});
        } else if (type == "multistring") {
            data.replacements[templStr] = TemplateItem(TemplateMultistring{label, templStr});
        } else if (type == "enum") {
            TemplateEnum e{label, templStr};
            for (const auto &value : repl.toObject().value("values").toArray()) {
                e.values.append(TemplateEnum::Value{value.toObject().value("key").toString(),
                                                    value.toObject().value("value").toString()});
            }
            data.replacements[templStr] = e;
        } else if (type == "header") {
            data.replacements[templStr] = TemplateItem(TemplateHeader{label});
        }
    }

    return data;
}
