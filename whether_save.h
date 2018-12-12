#ifndef WHETHER_SAVE_H
#define WHETHER_SAVE_H

#include <QDialog>

namespace Ui {
class WHETHER_SAVE;
}

class WHETHER_SAVE : public QDialog
{
    Q_OBJECT

public:
    explicit WHETHER_SAVE(QWidget *parent,const QString&head,const QString&question);
    ~WHETHER_SAVE();
signals:
    void SignalSave();
    void SignalDontSave();
    void SignalCancel();

private slots:
    void on_pushButton_save_clicked();

    void on_pushButton_dontsave_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui::WHETHER_SAVE *ui;
};

#endif // WHETHER_SAVE_H
