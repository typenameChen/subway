//站点信息修改窗口
#ifndef INFO_MODIFY_H
#define INFO_MODIFY_H

#include <QDialog>
class STATION;
namespace Ui {
class INFO_MODIFY;
}

class INFO_MODIFY : public QDialog
{
    Q_OBJECT
private:
    const STATION*p_station;
    QString ori_msg;

public:
    explicit INFO_MODIFY(const STATION*p_st, const QString&head,const QString&msg, QWidget *parent = 0);
    ~INFO_MODIFY();
signals:
    void SignalSave(const QString&msg);

private slots:
    void on_pushButton_save_clicked();

    void on_pushButton_quit_clicked();

private:
    Ui::INFO_MODIFY *ui;
};

#endif // INFO_MODIFY_H
