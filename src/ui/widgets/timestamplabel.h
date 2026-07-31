#ifndef TIMESTAMPLABEL_H
#define TIMESTAMPLABEL_H
#include <QDateTime>
#include <QLabel>

class TimestampLabel : public QLabel
{
    Q_OBJECT

    QDateTime m_timestamp;
    QString (*m_formatter)(const QString &formatted);

public:
    explicit TimestampLabel(QString (*formatter)(const QString &formatted) = nullptr, QWidget *parent = nullptr);

    [[nodiscard]] const QDateTime &timestamp() const;
    void setTimestamp(const QDateTime &timestamp);

    void updateText();
};

#endif //TIMESTAMPLABEL_H
