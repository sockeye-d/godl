#include "timestamplabel.h"

#include "dateconverter.h"

TimestampLabel::TimestampLabel(QString (*formatter)(const QString &formatted), QWidget *parent)
    : QLabel(parent)
    , m_formatter{formatter}
{}

const QDateTime &TimestampLabel::timestamp() const
{
    return m_timestamp;
}

void TimestampLabel::setTimestamp(const QDateTime &timestamp)
{
    if (timestamp == m_timestamp) {
        return;
    }

    m_timestamp = timestamp;
    updateText();
}

void TimestampLabel::updateText()
{
    if (m_formatter) {
        setText(m_formatter(DateConverter::relativeFormat(m_timestamp)));
    } else {
        setText(DateConverter::relativeFormat(m_timestamp));
    }

    setToolTip(m_timestamp.toLocalTime().toString(QLocale::system().dateTimeFormat(QLocale::LongFormat)));
}
