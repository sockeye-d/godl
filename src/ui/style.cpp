#include "style.h"

#include <QDebug>
#include <QFile>

QString Style::listViewStylesheet()
{
    static QString stylesheet = [] {
        QFile file{":/main.css"};
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qFatal() << "Couldn't open stylesheet";
        }

        return file.readAll();
    }();

    return stylesheet;
}