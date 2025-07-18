#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <QMetaObject>
#include <QString>
#include <qcontainerfwd.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

struct Template
{
    Q_GADGET
    QML_VALUE_TYPE(projectTemplate)
public:
    Q_PROPERTY(QString name MEMBER name CONSTANT FINAL)
    QString name{};
    Q_PROPERTY(QString path MEMBER path CONSTANT FINAL)
    QString path{};
    Q_PROPERTY(QString meta MEMBER meta CONSTANT FINAL)
    QString meta{};

    Template() {}
    Template(const QString &p_name, const QString &p_path);

    QString directory() const;
};

bool operator==(const Template &left, const Template &right);
void operator<<(QDebug &debug, const Template &t);

#endif // TEMPLATE_H
