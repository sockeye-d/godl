#ifndef PARSER_H
#define PARSER_H

#include <QDebug>
#include <QStringList>
#include "util.h"

class Parser
{
public:
    struct Option
    {
        friend class Parser;
        enum Mode {
            Switch,
            Command,
        };

    private:
        bool m_set = false;
        QStringList m_params = {};

    public:
        Option(Mode _mode = Switch,
               QString _name = "",
               QStringList _switches = {},
               QString _description = "",
               int _parameterCount = 0,
               QList<std::pair<QString, QString>> _parameterDescriptions = {})
            : mode{_mode}
            , name{_name}
            , switches{_switches}
            , description{_description}
            , parameterCount{_parameterCount}
            , parameterDescriptions{_parameterDescriptions}
        {}
        Mode mode = Switch;
        QString name = "";
        QStringList switches = {};
        QString description = "";
        int parameterCount = 0;
        QList<std::pair<QString, QString>> parameterDescriptions = {};

        bool set() const { return m_set; }
        const QStringList &params() const { return m_params; }
    };

    enum ParseResult {
        Success,
        UnknownArgument,
        UnknownCommand,
        TooFewArguments,
        HelpRequested,
    };

private:
    std::pair<ParseResult, int> parseArgument(const QString &argument,
                                              const QStringList &params,
                                              const Option::Mode &mode);

    QList<Option> m_options;
    QMap<QString, int> m_optionsByName;
    QMap<QString, int> m_optionsBySwitches;

    QList<int> filter(Option::Mode mode) const;

public:
    void addOption(Option option)
    {
        m_options.append(option);
        m_optionsByName[option.name] = m_options.size() - 1;
        for (const QString &s : std::as_const(option.switches)) {
            m_optionsBySwitches[s] = m_options.size() - 1;
        }
    }
    Parser();

    ParseResult parse(const QStringList &args, bool quiet = true);
    bool set(const QString &name) const;
    const QStringList &params(const QString &name) const;
    const Option &op(const QString name) const;
    QString helpText() const;

    const QList<Option> &options() const { return m_options; }
};

QDebug operator<<(QDebug left, const Parser::Option &option);

QDebug operator<<(QDebug left, const Parser &parser);

#endif // PARSER_H
