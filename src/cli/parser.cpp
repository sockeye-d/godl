#include "parser.h"
#include "cli/interface.h"
#include "util.h"

using namespace Qt::StringLiterals;

int Parser::allowedArgCount(int count, const QStringList &args)
{
    if (count == 0) {
        return 0;
    }
    if (count > args.size()) {
        return args.size();
    }
    int takenArgs = 0;
    for (int j = 0; j < count; j++) {
        const QString &arg = args[j];
        if (arg.startsWith("--")) {
            const auto &result = parseArgument(arg.sliced(2), args.sliced(j), Option::Switch);
            if (result.first == Parser::Success) {
                break;
            }
            takenArgs++;
        } else if (arg.startsWith("-")) {
            const QString s = arg.sliced(1);
            for (const QChar &subarg : s) {
                const auto &result = parseArgument(subarg, args.sliced(j), Option::Switch);
                if (result.first == Parser::Success) {
                    break;
                }
                takenArgs++;
            }
        } else {
            const auto &result = parseArgument(arg, args.sliced(j), Option::Command);
            if (result.first == Parser::Success) {
                break;
            }
            takenArgs++;
        }
        if (takenArgs >= count) {
            break;
        }
    }
    return takenArgs;
}

std::pair<Parser::ParseResult, int> Parser::parseArgument(const QString &argument,
                                                          const QStringList &params,
                                                          const Option::Mode &mode)
{
    const Parser::ParseResult unknownError = (mode == Option::Command ? Parser::UnknownCommand
                                                                      : Parser::UnknownArgument);
    if (!m_optionsBySwitches.contains(argument)) {
        return {unknownError, 0};
    }
    auto &op = m_options[m_optionsBySwitches.value(argument)];
    if (op.mode != mode) {
        return {unknownError, 0};
    }
    int reqCount = op.requiredParamCount();
    if (params.size() - 1 < reqCount) {
        return {Parser::TooFewParams, 0};
    }
    op.m_set = true;
    int argCount = allowedArgCount(op.parameterCount, params.sliced(1));
    if (argCount < reqCount) {
        return {Parser::TooFewParams, 0};
    }
    if (argCount > op.parameterCount) {
        op.m_params = params.sliced(1, op.parameterCount);
    } else {
        op.m_params = params.sliced(1, argCount);
    }
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

QList<QStringList> processHelpText(const Parser::Option &op)
{
    static const QString &indent = "  ";
    QList<QStringList> columns;
    QStringList shortParams;
    for (const auto &param : op.parameters) {
        if (param.optional) {
            shortParams.append(u"[%1]"_s.arg(param.name));
        } else {
            shortParams.append(u"<%1>"_s.arg(param.name));
        }
    }
    QString shortParamsStr = shortParams.empty() ? "" : " " + shortParams.join(" ");
    QString switches;
    if (op.mode == Parser::Option::Switch) {
        for (const QString &s : op.switches) {
            if (s != op.switches.constFirst()) {
                switches += ", ";
            }
            if (s.length() == 1) {
                switches += "-" + s;
            } else {
                switches += "--" + s;
            }
        }
    } else {
        switches = op.switches.join(", ");
    }
    columns.append({indent + switches + shortParamsStr, " " + op.description});
    bool actuallyAppendedRows = false;
    for (const auto &param : std::as_const(op.parameters)) {
        if (!param.description.isEmpty()) {
            actuallyAppendedRows = true;
            if (param.optional) {
                columns.append({"", u" [%1] (optional) %2"_s.arg(param.name, param.description)});
            } else {
                columns.append({"", u" <%1> %2"_s.arg(param.name, param.description)});
            }
        }
    }
    if (actuallyAppendedRows) {
        columns.append({""});
    }
    return columns;
}

QString Parser::helpText() const
{
    static const QString &indent = "  ";
    QList<QStringList> columns;
    columns.append({"Commands:"});
    for (const int i : filter(Option::Command)) { // clazy:exclude=range-loop-detach
        columns.append(processHelpText(m_options[i]));
    }
    columns.append({"Options:"});
    for (const int i : filter(Option::Switch)) { // clazy:exclude=range-loop-detach
        columns.append(processHelpText(m_options[i]));
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

Parser::Option::Option(Mode _mode,
                       QString _name,
                       QStringList _switches,
                       QString _description,
                       QList<Param> _parameterDescriptions)
    : mode{_mode}
    , name{_name}
    , switches{_switches}
    , description{_description}
    , parameters{_parameterDescriptions}
{
    parameterCount = _parameterDescriptions.size();
    bool atOptional = false;
    for (const Param &param : std::as_const(parameters)) {
        if (param.optional) {
            atOptional = true;
        }
        Q_ASSERT(param.optional == atOptional);
    }
}

const QString &Parser::Option::param(const QString &name, const QString &defaultValue) const
{
    for (int i = 0; i < parameters.size(); i++) {
        if (i >= m_params.size()) {
            break;
        }
        if (parameters[i].name == name) {
            return m_params[i];
        }
    }
    return defaultValue;
}

int Parser::Option::requiredParamCount() const
{
    for (int i = 0; i < parameters.size(); i++) {
        if (parameters[i].optional) {
            return i;
        }
    }
    return parameters.size();
}
