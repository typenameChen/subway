//移动站点操作窗口  负责传递位置信息
#ifndef DIALOG_MOVE_H
#define DIALOG_MOVE_H

#include <QDialog>

namespace Ui {
class DIALOG_MOVE;
}

class DIALOG_MOVE : public QDialog
{
    Q_OBJECT
private:
    int max_value;
    int min_value;

public:
    explicit DIALOG_MOVE(const QString&st_name, int st_count,QWidget *parent = 0);
    ~DIALOG_MOVE();
signals:
    void SignalPos(int pos);//附带移入位置的信号

private slots:
    void on_pushButton_ok_clicked();//  complete

    void on_pushButton_cancel_clicked();//  complete

private:
    Ui::DIALOG_MOVE *ui;
};

#endif // DIALOG_MOVE_H
