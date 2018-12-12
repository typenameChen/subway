#ifndef DIALOG_ADD_LINE_H
#define DIALOG_ADD_LINE_H

#include <QDialog>

namespace Ui {
class DIALOG_ADD_LINE;
}

class DIALOG_CREAT_LINE : public QDialog
{
    Q_OBJECT

public:
    explicit DIALOG_CREAT_LINE(QWidget *parent = 0);
    void SetLabelText(const QString&msg)const;
    ~DIALOG_CREAT_LINE();
signals:
    void SignalGetLineName(const QString&line);

private slots:

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::DIALOG_ADD_LINE *ui;
};

#endif // DIALOG_ADD_LINE_H
