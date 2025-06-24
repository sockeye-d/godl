#ifndef BETTERFILEDIALOG_H
#define BETTERFILEDIALOG_H

#include <QFileDialog>
#include <QMetaEnum>
#include <QtQml/qqmlregistration.h>

/**
 * @brief Exposes \ref QFileDialog to QML. Is this cursed? Maybe.
 */
class BetterFileDialog : public QObject
{
    Q_OBJECT
    QML_ELEMENT

private:
    Q_PROPERTY(QStringList filters READ filters WRITE setFilters NOTIFY filtersChanged FINAL)
    Q_PROPERTY(QDir::Filters fileFilters READ fileFilters WRITE setFileFilters NOTIFY
                   fileFiltersChanged FINAL)
    Q_PROPERTY(QFileDialog::Options options READ options WRITE setOptions NOTIFY optionsChanged FINAL)
    Q_PROPERTY(QVariantList labels READ labels WRITE setLabels NOTIFY labelsChanged FINAL)
    Q_PROPERTY(QString startDirectory READ startDirectory WRITE setStartDirectory NOTIFY
                   startDirectoryChanged FINAL)
    Q_PROPERTY(QFileDialog::FileMode mode READ mode WRITE setMode NOTIFY modeChanged FINAL)

    QVariantList m_labels = QVariantList(5);
    QDir::Filters m_fileFilters = QDir::AllEntries;
    QFileDialog::Options m_options = (QFileDialog::Options) 0;
    QStringList m_filters = {};
    QString m_startDirectory = "";
    QFileDialog::FileMode m_mode = QFileDialog::AnyFile;

public:
    enum Filter {
        Dirs = 0x001,
        Files = 0x002,
        Drives = 0x004,
        NoSymLinks = 0x008,
        AllEntries = Dirs | Files | Drives,
        TypeMask = 0x00f,

        Readable = 0x010,
        Writable = 0x020,
        Executable = 0x040,
        PermissionMask = 0x070,

        Modified = 0x080,
        Hidden = 0x100,
        System = 0x200,

        AccessMask = 0x3F0,

        AllDirs = 0x400,
        CaseSensitive = 0x800,
        NoDot = 0x2000,
        NoDotDot = 0x4000,
        NoDotAndDotDot = NoDot | NoDotDot,

        NoFilter = -1
    };
    Q_ENUM(Filter)
    Q_DECLARE_FLAGS(Filters, Filter)
    Q_ENUM(Filters)

    BetterFileDialog(QObject *parent = nullptr)
        : QObject(parent)
    {}

    QStringList filters() const { return m_filters; }
    void setFilters(QStringList filters);

    void setFileFilters(QDir::Filters fileFilters);
    QDir::Filters fileFilters() const { return m_fileFilters; }

    QFileDialog::Options options() const { return m_options; }
    void setOptions(QFileDialog::Options options);

    void setLabels(QVariantList labels);
    void setLabel(QFileDialog::DialogLabel label, const QString &text);
    const QVariantList labels() const { return m_labels; }

    QString startDirectory() const { return m_startDirectory; }
    void setStartDirectory(QString startDirectory);

    void setMode(QFileDialog::FileMode mode);
    QFileDialog::FileMode mode() const { return m_mode; }

    Q_INVOKABLE void open();

    Q_SIGNAL void filtersChanged();
    Q_SIGNAL void fileFiltersChanged();
    Q_SIGNAL void optionsChanged();
    Q_SIGNAL void labelsChanged();
    Q_SIGNAL void startDirectoryChanged();
    Q_SIGNAL void modeChanged();

    Q_SIGNAL void accepted(QString directory);
    Q_SIGNAL void canceled();
};

#endif // BETTERFILEDIALOG_H
