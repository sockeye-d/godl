#ifndef FILELINEEDIT_H
#define FILELINEEDIT_H

#include <QWidget>

class QFileDialog;
class QLineEdit;

class FileLineEdit  : public QWidget
{
    Q_OBJECT

    QLineEdit *m_lineEdit;
    QFileDialog *m_fileDialog;

public:
    [[nodiscard]] QLineEdit *lineEdit() const;
    [[nodiscard]] QFileDialog *fileDialog() const;

    explicit FileLineEdit(QWidget *parent = nullptr);
    ~FileLineEdit() override;
};

#endif //FILELINEEDIT_H
