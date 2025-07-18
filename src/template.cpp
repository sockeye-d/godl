#include "template.h"

#include <QDebug>
#include <QFile>

bool operator==(const Template &left, const Template &right)
{
    return left.name == right.name && left.path == right.path && left.meta == right.meta;
}

Template::Template(const QString &p_name, const QString &p_path)
    : name{p_name}
    , path{p_path}
{
    QFile f{p_path};
    f.open(QFile::ReadOnly | QFile::Text);
    meta = QString::fromUtf8(f.readAll());
}

void operator<<(QDebug &debug, const Template &t)
{
    debug << t.name;
}
