#include "parser.h"
#include "cli/interface.h"
#include "util.h"

using namespace Qt::StringLiterals;

bool Parser::canTakeArgs(int count, const QStringList &args)
{
    if (count == 0) {
        return true;
    }
    if (count > args.size()) {
        return false;
    }
    int takenArgs = 0;
    for (int j = 0; j < count; j++) {
        const QString &arg = args[j];
        if (arg.startsWith("--")) {
            const auto &result = parseArgument(arg.sliced(2), args.sliced(j), Option::Switch);
            if (result.first == Parser::Success) {
                return false;
            }
            takenArgs++;
        } else if (arg.startsWith("-")) {
            const QString s = arg.sliced(1);
            for (const QChar &subarg : s) {
                const auto &result = parseArgument(subarg, args.sliced(j), Option::Switch);
                if (result.first == Parser::Success) {
                    return false;
                }
                takenArgs++;
            }
        } else {
            const auto &result = parseArgument(arg, args.sliced(j), Option::Command);
            if (result.first == Parser::Success) {
                return false;
            }
            takenArgs++;
        }
        if (takenArgs >= count) {
            break;
        }
    }
    if (takenArgs < count) {
        return false;
    }
    return true;
}

#define UnknownError (mode == Option::Command ? Parser::UnknownCommand : Parser::UnknownArgument)
std::pair<Parser::ParseResult, int> Parser::parseArgument(const QString &argument,
                                                          const QStringList &params,
                                                          const Option::Mode &mode)
{
    if (!m_optionsBySwitches.contains(argument)) {
        return {UnknownError, 0};
    }
    auto &op = m_options[m_optionsBySwitches.value(argument)];
    if (op.mode != mode) {
        return {UnknownError, 0};
    }
    if (params.size() - 1 < op.parameterCount) {
        return {Parser::TooFewParams, 0};
    }
    op.m_set = true;
    if (!canTakeArgs(op.parameterCount, params.sliced(1))) {
        print_debug() << op.parameterCount << op.name << params;
        return {Parser::TooFewParams, 0};
    }
    op.m_params = params.sliced(1, op.parameterCount);
    return {Parser::Success, op.parameterCount};
}
#undef UnknownError

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

void Parser::addOption(Option option)
{
    m_optionsByName[option.name] = m_options.size();
    for (const QString &s : std::as_const(option.switches)) {
        m_optionsBySwitches[s] = m_options.size();
    }
    m_options.append(option);
}

void Parser::clearCommands(const QSet<QString> &exclusions)
{
    QList<int> opsToRemove;

    for (int i = 0; i < m_options.size(); i++) {
        const Option &op = m_options[i];
        if (op.mode != Option::Command) {
            continue;
        }
        if (exclusions.contains(op.name)) {
            continue;
        }
        opsToRemove.append(i);
    }
    for (int i = 0; i < opsToRemove.size(); i++) {
        m_options.remove(opsToRemove[i] - i);
    }

    m_optionsBySwitches.clear();
    m_optionsByName.clear();

    for (int i = 0; i < m_options.size(); i++) {
        for (const QString &s : std::as_const(m_options[i].switches)) {
            m_optionsBySwitches[s] = i;
        }
        m_optionsByName[m_options[i].name] = i;
    }
}

Parser::Parser() {}

#define handleResult(error, argument) \
    if (!quiet) { \
        switch (error) { \
        case Error: \
        case UnknownArgument: \
            m_errors << u"unknown argument \"%1\""_s.arg(argument); \
            break; \
        case UnknownCommand: \
            m_errors << u"unknown command \"%1\""_s.arg(argument); \
            break; \
        case TooFewParams: \
            m_errors << u"too few parameters for \"%1\""_s.arg(argument); \
            break; \
        case Success: \
        case HelpRequested: \
            break; \
        } \
    }

Parser::ParseResult Parser::parse(const QStringList &args, bool quiet)
{
    m_errors.clear();
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

    if (!m_errors.empty()) {
        return Parser::Error;
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
    QList<QStringList> columns;
    columns.append({"Commands:"});
    for (const int i : filter(Option::Command)) { // clazy:exclude=range-loop-detach
        const Option &op = m_options[i];
        QStringList shortParams;
        for (const auto &param : op.parameterDescriptions) {
            shortParams.append(u"<%1>"_s.arg(param.first));
        }
        QString shortParamsStr = shortParams.empty() ? "" : " " + shortParams.join(" ");
        columns.append({indent + op.switches.join(", ") + shortParamsStr, " " + op.description});
        bool actuallyAppendedRows = false;
        for (const auto &param : std::as_const(op.parameterDescriptions)) {
            if (!param.second.isEmpty()) {
                actuallyAppendedRows = true;
                columns.append({"", u" <%1> %2"_s.arg(param.first, param.second)});
            }
        }
        if (actuallyAppendedRows) {
            columns.append({""});
        }
    }
    columns.append({"Options:"});
    for (const int i : filter(Option::Switch)) { // clazy:exclude=range-loop-detach
        const Option &op = m_options[i];
        QStringList shortParams;
        for (const auto &param : op.parameterDescriptions) {
            shortParams.append(u"<%1>"_s.arg(param.first));
        }
        QString shortParamsStr = shortParams.empty() ? "" : " " + shortParams.join(" ");
        QStringList switches;
        for (const QString &s : op.switches) {
            if (s.length() <= 1) {
                switches.append("-" + s);
            }
        }
        for (const QString &s : op.switches) {
            if (s.length() > 1) {
                switches.append("--" + s);
            }
        }
        columns.append({indent + switches.join(", ") + shortParamsStr, " " + op.description});
        bool actuallyAppendedRows = false;
        for (const auto &param : std::as_const(op.parameterDescriptions)) {
            if (!param.second.isEmpty()) {
                actuallyAppendedRows = true;
                columns.append({"", u" <%1> %2"_s.arg(param.first, param.second)});
            }
        }
        if (actuallyAppendedRows) {
            columns.append({""});
        }
    }
    return cli::asColumns(columns);
}

const Parser::Option &Parser::op(const QString name) const
{
    return m_options[m_optionsByName.value(name)];
}

QDebug operator<<(QDebug left, const Parser &parser)
{
    left.nospace().noquote() << "Parser(options=" << parser.options() << ")";
    return left;
}

QDebug operator<<(QDebug left, const Parser::Option &option)
{
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

const QString &Parser::Option::param(const QString &name, const QString &defaultValue) const
{
    for (int i = 0; i < parameterDescriptions.size(); i++) {
        if (i >= m_params.size()) {
            break;
        }
        if (parameterDescriptions[i].first == name) {
            return m_params[i];
        }
    }
    return defaultValue;
}
