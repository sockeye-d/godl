#include "createcommand.h"
#include "cli/ansi.h"
#include "cli/cliutil.h"
#include "cli/interface.h"
#include "projectsregistry.h"
#include "projecttemplates.h"
#include "template.h"
#include "templatedata.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

enum VerificationResult {
    Valid,
    Invalid,
    CompletelyInvalid,
};

VerificationResult verifyParameter(const TemplateData &data,
                                   const QString &name,
                                   const QString &value,
                                   bool quiet = false)
{
    TemplateItem varItem = data.replacements.value(name);
    if (TemplateEnum *item = std::get_if<TemplateEnum>(&varItem)) {
        QStringList allowedValues;
        for (const auto &value : std::as_const(item->values)) {
            allowedValues.append(value.key);
        }
        if (allowedValues.contains(value)) {
            return Valid;
        }
        if (!quiet)
            qStdOut() << cli::error() << "value " << value << " not allowed (possible values are "
                      << allowedValues.join(", ") << ")" << cli::ansi::nl;
        return CompletelyInvalid;
    }
    if (name == "name" && value.isEmpty()) {
        return Invalid;
    }
    return Valid;
}

int cli::create(const Parser &parser)
{
    const QString &templateName = parser.op("create").param("template");
    bool force = parser.set("force");
    if (!ProjectTemplates::instance()->hasTemplate(templateName)) {
        qStdOut() << error() << "can't find template " << templateName;
        return 1;
    }
    QMap<QString, QString> arguments;
    arguments["name"] = parser.op("name").param("name",
                                                ProjectTemplates::instance()->generateRandomName());
    arguments["path"] = makeAbsolute(parser.op("path").param("path", arguments["name"]));
    Template templ = ProjectTemplates::instance()->templ(templateName);
    TemplateData data = TemplateData::parse(templ.meta);

    for (TemplateItem &uitem : data.replacements) {
        if (auto item = std::get_if<TemplateString>(&uitem)) {
            QString arg = "t-" + item->templ;
            arguments[item->templ] = parser.op(arg).param("value");
        } else if (auto item = std::get_if<TemplateMultistring>(&uitem)) {
            QString arg = "t-" + item->templ;
            arguments[item->templ] = parser.op(arg).param("value");
        } else if (auto item = std::get_if<TemplateEnum>(&uitem)) {
            QStringList allowedValues;
            for (const auto &value : std::as_const(item->values)) {
                allowedValues.append(value.key);
            }
            QString arg = "t-" + item->templ;
            if (!parser.set(arg)) {
                arguments[item->templ] = allowedValues.constFirst();
                continue;
            }
            QString value = parser.op(arg).param("value");
            if (!allowedValues.contains(value)) {
                qStdOut() << error() << "value " << value << " not allowed (possible values are "
                          << allowedValues.join(", ") << ")" << ansi::nl;
                return 1;
            }
            arguments[item->templ] = value;
        }
    }

    int lastDrawnRows = -1;
    bool valid = true;
    QStringList invalidParams;
    for (const auto &[arg, value] : arguments.asKeyValueRange()) {
        auto result = verifyParameter(data, arg, value);
        if (result == CompletelyInvalid) {
            return 1;
        }

        if (result == Invalid) {
            valid = false;
            invalidParams.append(arg);
            break;
        }
    }

    bool skipMenu = parser.set("skip-menu");

    if (skipMenu && !valid) {
        qStdOut() << error() << "failed to skip menu, " << invalidParams.join(", ")
                  << " were invalid";
        return 1;
    }

    if (!valid || !skipMenu) {
        qStdOut() << note() << "skip the menu with --skip-menu" << ansi::nl;
        while (true) {
            Table table;
            QMap<QString, int> rowMap;
            QMap<QString, QString> shorthands;
            valid = true;
            for (int i = 0; const auto &[arg, value] : arguments.asKeyValueRange()) {
                const TemplateItem &item = data.replacements.value(arg);
                QString valueString = "";
                if (auto e = std::get_if<TemplateEnum>(&item)) {
                    QStringList values;
                    for (const auto &possibleValue : e->values) {
                        if (possibleValue.key == value) {
                            values.append("[" % ansi::fgcolor(ansi::Green) % possibleValue.key
                                          % ansi::reset % "]");
                        } else {
                            values.append(QStringLiteral(" %1 ").arg(possibleValue.key));
                        }
                    }
                    valueString = values.join(" ");
                } else {
                    valueString = value;
                }
                bool thisValid = verifyParameter(data, arg, value, true);
                valid &= thisValid == Valid;
                table.append({(thisValid == Valid ? "" : ansi::fgcolor(ansi::Red))
                                  + ansi::underline(arg, 0, 3) + ansi::reset + ":",
                              valueString});
                rowMap[arg] = i;
                shorthands[arg.sliced(0, 3)] = arg;
                i++;
            }
            if (lastDrawnRows >= 0) {
                qStdOut() << ansi::cr << ansi::cursorMove(ansi::Up, lastDrawnRows);
            }
            lastDrawnRows = table.size();
            qStdOut() << asColumns(table, " ", false, ansi::eraseInLine);
            qStdOut().flush();

            QStringList keys;
            keys.reserve(arguments.size());
            for (const auto &[key, _] : arguments.asKeyValueRange()) {
                keys.append(key.sliced(0, 3));
            }
            if (valid)
                keys << "done";
            // prompt for key
            QString input = prompt(ansi::eraseInLine + "which key to edit?", keys);
            // QString input = shorthands.value("nam");
            if (input == "done") {
                break;
            }
            input = shorthands.value(input);
            lastDrawnRows++;
            int movedUpRows = lastDrawnRows - rowMap[input];
            qStdOut() << ansi::cursorMove(ansi::Up) << ansi::eraseInLine
                      << ansi::cursorMove(ansi::Up, movedUpRows - 1) << ansi::cr;
            QString value;
            const TemplateItem selectedItem = data.replacements.value(input);

            // prompt for value
            if (auto _ = std::get_if<TemplateString>(&selectedItem)) {
                value = prompt(input + ":");
            } else if (auto _ = std::get_if<TemplateMultistring>(&selectedItem)) {
                value = prompt(input + ":");
            } else if (auto item = std::get_if<TemplateEnum>(&selectedItem)) {
                QStringList values;
                for (const auto &possibleValue : item->values) {
                    values.append(possibleValue.key);
                }
                value = prompt(input, values);
            } else {
                value = prompt(input + ":");
            }
            arguments[input] = value;
            if (input == "name") {
                arguments["path"] = makeAbsolute(parser.op("path").param("path", arguments["name"]));
            }
            qStdOut() << ansi::cursorMove(ansi::Down, movedUpRows) << ansi::cr;
            qStdOut().flush();
        }
    }

    if (QDir(arguments.value("path")).exists()) {
        if (force) {
            qStdOut() << note() << "force-creating project directory" << ansi::nl;
        } else {
            qStdOut() << error() << "project path exists" << ansi::nl;
            qStdOut() << note() << "override with --force" << ansi::nl;
            bool confirm = prompt("Are you sure you want to continue?", {"y", "n"}, 1) == "y";
            if (!confirm) {
                return 1;
            }
        }
    }

    QVariantMap replacements;
    for (const auto &[key, val] : arguments.asKeyValueRange()) {
        if (key == "path") {
            continue;
        }
        replacements[key] = val;
    }
    QString path = arguments.value("path");
    ProjectTemplates::instance()->createProject(templateName, path, replacements);
    qStdOut() << positive() << "created project from template at " << path << flushnl();
    ProjectsRegistry::instance()->import(path);
    qStdOut() << positive() << "imported project at " << path << flushnl();

    return 0;
}

int cli::list(const Parser &)
{
    for (const Template &t : // clazy:exclude=range-loop-detach
         ProjectTemplates::instance()->templates()) {
        qStdOut() << t.name << ansi::nl;
    }
    return 0;
}

bool cli::addTemplateParams(Parser &parser)
{
    using Option = Parser::Option;
    using enum Option::Mode;
    const QString &templateName = parser.op("create").param("template");
    if (!ProjectTemplates::instance()->hasTemplate(templateName)) {
        qStdOut() << error() << "can't find template " << templateName;
        return true;
    }

    Template templ = ProjectTemplates::instance()->templ(templateName);
    TemplateData data = TemplateData::parse(templ.meta);

    for (TemplateItem &item : data.replacements) {
        if (auto string = std::get_if<TemplateString>(&item)) {
            parser.addOption(Option(Switch,
                                    "t-" + string->templ,
                                    {"t-" + string->templ},
                                    string->label + " (Template replacement)",
                                    {{"value"}}));
        } else if (auto string = std::get_if<TemplateMultistring>(&item)) {
            parser.addOption(Option(Switch,
                                    "t-" + string->templ,
                                    {"t-" + string->templ},
                                    string->label + " (Template replacement)",
                                    {{"value"}}));
        } else if (auto tenum = std::get_if<TemplateEnum>(&item)) {
            QStringList allowedValues;
            for (const auto &value : std::as_const(tenum->values)) {
                allowedValues.append(value.key);
            }
            parser.addOption(Option(Switch,
                                    "t-" + tenum->templ,
                                    {"t-" + tenum->templ},
                                    tenum->label + " (enum) (Template replacement)",
                                    {{"value", "one of " + allowedValues.join(", ")}}));
        }
    }

    return false;
}
