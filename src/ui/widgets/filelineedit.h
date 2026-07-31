#ifndef FILELINEEDIT_H
#define FILELINEEDIT_H

#include <QLineEdit>
#include <QWidget>

class QFileDialog;
class QLineEdit;

class FileLineEdit  : public QLineEdit
{
    Q_OBJECT

    // QLineEdit *m_lineEdit;
    QFileDialog *m_fileDialog;

public:
    [[nodiscard]] QLineEdit *lineEdit();
    [[nodiscard]] QFileDialog *fileDialog() const;

    explicit FileLineEdit(QWidget *parent = nullptr);
    ~FileLineEdit() override;
};

#endif //FILELINEEDIT_H
