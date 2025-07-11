#include "betterfiledialog.h"

#include <QImageReader>

void BetterFileDialog::setOptions(QFileDialog::Options options)
{
    if (m_options == options)
        return;
    m_options = options;
    Q_EMIT optionsChanged();
}

void BetterFileDialog::setLabels(QVariantList labels)
{
    if (m_labels == labels)
        return;
    m_labels = labels;
    Q_EMIT labelsChanged();
}

void BetterFileDialog::setLabel(QFileDialog::DialogLabel label, const QString &text)
{
    m_labels[label] = text;
}

void BetterFileDialog::open()
{
    auto dg = new QFileDialog();
    QStringList newFilters = filters();
    auto imageIndex = newFilters.indexOf("filter/images");
    if (imageIndex != -1) {
        QString str = "Images (";
        for (const auto &fmt : QImageReader::supportedImageFormats()) {
            qDebug() << fmt;
            str = str % "*." % fmt % " ";
        }
        newFilters.replace(imageIndex, str + ")");
    }
    dg->setNameFilters(newFilters);
    dg->setFilter(fileFilters());
    QFileDialog::Options op = options();
    if (fileFilters() & QDir::Hidden) {
        op |= QFileDialog::DontUseNativeDialog;
    }
    dg->setOptions(op);

    int i = 0;
    for (const auto &label : labels()) {
        if (!label.isNull()) {
            dg->setLabelText((QFileDialog::DialogLabel) i, label.toString());
        }
        i++;
    }

    dg->setDirectory(startDirectory());

    dg->setFileMode(mode());

    int result = dg->exec();
    if (result == QDialog::Accepted && dg->selectedUrls().constFirst().isLocalFile()) {
        Q_EMIT accepted(dg->selectedUrls().constFirst().toLocalFile());
    } else {
        Q_EMIT canceled();
    }
    dg->deleteLater();
}

void BetterFileDialog::setStartDirectory(QString startDirectory)
{
    if (m_startDirectory == startDirectory)
        return;
    m_startDirectory = startDirectory;
    Q_EMIT startDirectoryChanged();
}

void BetterFileDialog::setFilters(QStringList filters)
{
    if (m_filters == filters)
        return;
    m_filters = filters;
    Q_EMIT filtersChanged();
}

void BetterFileDialog::setFileFilters(QDir::Filters fileFilters)
{
    if (m_fileFilters == fileFilters)
        return;
    m_fileFilters = fileFilters;
    Q_EMIT fileFiltersChanged();
}

void BetterFileDialog::setMode(QFileDialog::FileMode mode)
{
    if (m_mode == mode)
        return;
    m_mode = mode;
    Q_EMIT modeChanged();
}
