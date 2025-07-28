#ifndef PARSER_H
#define PARSER_H

#include <QMap>
#include <QSet>
#include <QStringList>

class Parser
{
public:
    struct Param
    {
        QString name;
        QString description = "";
        bool optional = false;
    };

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
               QList<Param> _parameterDescriptions = {});
        Mode mode = Switch;
        QString name = "";
        QStringList switches = {};
        QString description = "";
        int parameterCount = 0;
        QList<Param> parameters = {};

        bool set() const { return m_set; }
        const QStringList &params() const { return m_params; }
        const QString &param(const QString &name, const QString &defaultValue = "") const;
        bool hasParam(const QString &name) const;
        int requiredParamCount() const;
    };

    enum ParseResult {
        Success,
        Error,
        UnknownArgument,
        UnknownCommand,
        TooFewParams,
        HelpRequested,
    };

private:
    std::pair<ParseResult, int> parseArgument(const QString &argument,
                                              const QStringList &params,
                                              const Option::Mode &mode);

    QList<Option> m_options;
    QMap<QString, int> m_optionsByName;
    QMap<QString, int> m_optionsBySwitches;
    QStringList m_errors;

    QList<int> filter(Option::Mode mode) const;

    int allowedArgCount(int count, const QStringList &args);

public:
    void addOption(Option option);
    void clearCommands(const QSet<QString> &exclusions = {});
    Parser();

    ParseResult parse(const QStringList &args, bool quiet = true);
    const QStringList &errors() const { return m_errors; }
    bool set(const QString &name) const;
    const QStringList &params(const QString &name) const;
    const Option &op(const QString name) const;
    QString helpText() const;

    const QList<Option> &options() const { return m_options; }
};

QDebug operator<<(QDebug left, const Parser::Option &option);

QDebug operator<<(QDebug left, const Parser &parser);

#endif // PARSER_H
