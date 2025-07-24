#include "parser.h"

std::pair<Parser::ParseResult, int> Parser::parseArgument(const QString &argument,
                                                          const QStringList &params,
                                                          const Option::Mode &mode)
{
    if (!m_optionsBySwitches.contains(argument)) {
        return {mode == Option::Command ? Parser::UnknownCommand : Parser::UnknownArgument, 0};
    }
    auto &op = m_options[m_optionsBySwitches.value(argument)];
    if (op.mode != mode) {
        return {mode == Option::Command ? Parser::UnknownCommand : Parser::UnknownArgument, 0};
    }
    if (params.size() - 1 < op.parameterCount) {
        return {Parser::TooFewArguments, 0};
    }
    op.m_set = true;
    op.m_params = params.sliced(1, op.parameterCount);
    return {Parser::Success, op.parameterCount};
}

QList<int> Parser::filter(Option::Mode mode) const
{
    QList<int> ops;
    for (int i = 0; i < m_options.size(); i++) {
        if (m_options[i].mode == mode) {
            ops.append(i);
        }
    }
    return ops;
}

Parser::Parser() {}

#define handleResult(error, argument) \
    if (!quiet) { \
        switch (error) { \
        case UnknownArgument: \
            print_clean() << "Unknown argument" << argument; \
            return error; \
        case UnknownCommand: \
            print_clean() << "Unknown command" << argument; \
            return error; \
        case TooFewArguments: \
            print_clean() << "Too few parameters for" << argument; \
            return error; \
        case Success: \
        case HelpRequested: \
            break; \
        } \
    }

Parser::ParseResult Parser::parse(const QStringList &args, bool quiet)
{
    for (Option &op : m_options) {
        op.m_set = false;
        op.m_params.clear();
    }

    for (int i = 0; i < args.size(); i++) {
        const QString &arg = args[i];
        if (arg.startsWith("--")) {
            // long switch
            const QString s = arg.sliced(2);
            const auto &result = parseArgument(s, args.sliced(i), Option::Switch);
            handleResult(result.first, s);
            i += result.second;
        } else if (arg.startsWith("-")) {
            // short combined switch
            const QString s = arg.sliced(1);
            for (const QChar &subarg : s) {
                const auto &result = parseArgument(subarg, args.sliced(i), Option::Switch);
                handleResult(result.first, subarg);
                i += result.second;
            }
        } else {
            // command
            const auto &result = parseArgument(arg, args.sliced(i), Option::Command);
            handleResult(result.first, arg);
            i += result.second;
        }
    }

    if (set("help") && !quiet) {
        print_clean().noquote() << helpText();
        return Parser::HelpRequested;
    }

    return Parser::Success;
}

bool Parser::set(const QString &name) const
{
    return m_options[m_optionsByName.value(name)].set();
}

const QStringList &Parser::params(const QString &name) const
{
    return m_options[m_optionsByName.value(name)].params();
}

QString Parser::helpText() const
{
    static const QString &indent = "  ";
    using namespace Qt::StringLiterals;
    QList<QStringList> columns;
    columns.append({"Commands:"});
    for (const int i : filter(Option::Command)) { // clazy:exclude=range-loop-detach
        QStringList shortParams;
        QStringList longParams;
        for (const auto &param : m_options[i].parameterDescriptions) {
            shortParams.append(u"<%1>"_s.arg(param.first));
            longParams.append(u"<%1> %2"_s.arg(param.first, param.second));
        }
        QString shortParamsStr = shortParams.empty() ? "" : " " + shortParams.join(" ");
        columns.append({indent + m_options[i].switches.join(", ") + shortParamsStr,
                        " " + m_options[i].description});
        for (const QString &param : std::as_const(longParams)) {
            columns.append({"", " " + param});
        }
        if (!longParams.empty()) {
            columns.append({""});
        }
    }
    columns.append({"Options:"});
    for (const int i : filter(Option::Switch)) { // clazy:exclude=range-loop-detach
        QStringList shortParams;
        QStringList longParams;
        for (const auto &param : m_options[i].parameterDescriptions) {
            shortParams.append(u"<%1>"_s.arg(param.first));
            longParams.append(u"<%1> %2"_s.arg(param.first, param.second));
        }
        QString shortParamsStr = shortParams.empty() ? "" : " " + shortParams.join(" ");
        QStringList switches;
        for (const QString &s : m_options[i].switches) {
            if (s.length() <= 1) {
                switches.append("-" + s);
            }
        }
        for (const QString &s : m_options[i].switches) {
            if (s.length() > 1) {
                switches.append("--" + s);
            }
        }
        columns.append(
            {indent + switches.join(", ") + shortParamsStr, " " + m_options[i].description});
        for (const QString &param : std::as_const(longParams)) {
            columns.append({"", " " + param});
        }
        if (!longParams.empty()) {
            columns.append({""});
        }
    }
    QList<int> columnWidths;
    for (int rowIndex = 0; rowIndex < columns.size(); rowIndex++) {
        const QStringList &row = columns[rowIndex];
        if (row.size() > columnWidths.size()) {
            columnWidths.resize(row.size());
        }

        for (int colIndex = 0; colIndex < row.size(); colIndex++) {
            const QString &col = row[colIndex];
            if (col.length() > columnWidths[colIndex]) {
                columnWidths[colIndex] = col.length();
            }
        }
    }
    int totalWidth = 0;
    for (const int &width : std::as_const(columnWidths))
        totalWidth += width;
    QString result;
    result.reserve(totalWidth * columns.size());
    for (const QStringList &column : std::as_const(columns)) {
        for (int i = 0; i < column.size(); i++) {
            result += u"%1"_s.arg(column[i], -columnWidths[i]);
        }
        result += "\n";
    }
    return result;
}

const Parser::Option &Parser::op(const QString name) const
{
    return m_options[m_optionsByName.value(name)];
}

QDebug operator<<(QDebug left, const Parser &parser)
{
    using namespace Qt::StringLiterals;
    left.nospace().noquote() << "Parser(options=" << parser.options() << ")";
    return left;
}

QDebug operator<<(QDebug left, const Parser::Option &option)
{
    using namespace Qt::StringLiterals;
    if (option.parameterCount == 0) {
        left.nospace().noquote() << u"Option(option=%1, set=%2)"_s.arg(option.name).arg(option.set());
    } else {
        ((left.nospace().noquote() << u"Option(option=%1, params="_s.arg(option.name)).quote()
         << option.params())
                .noquote()
            << ")";
    }
    return left;
}
