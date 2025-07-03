#include "dateconverter.h"

#include <QDate>

DateConverter::DateConverter(QObject *parent)
    : QObject{parent}
{}

QString DateConverter::relativeFormat(QDateTime date)
{
    using std::chrono::milliseconds;
    using std::chrono::seconds;
    using namespace std::chrono_literals;
    using std::chrono::duration_cast;
    using namespace Qt::StringLiterals;
    // hmm nice triangle
    static QMap<seconds, QString> times = {
        {1s, "second"},
        {60s, "minute"},
        {3600s, "hour"},
        {86400s, "day"},
        {604800s, "week"},
        {2592000s, "month"},
        {31536000s, "year"},
    };

    auto offset = duration_cast<seconds>(QDateTime::currentDateTime() - date);

    seconds duration = 1s;
    for (auto time : times.asKeyValueRange()) {
        if (offset > time.first) {
            duration = time.first;
        }
    }

    const int value = (int) (std::round(offset / duration));
    const QString t = offset.count() > 0 ? "%1 ago" : "in %1";
    return t.arg(QString::number(value) % " " % times[duration] % (value == 1 ? "" : "s"));
}
