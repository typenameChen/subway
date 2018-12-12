#include "dialog_move.h"
#include "ui_dialog_move.h"

DIALOG_MOVE::DIALOG_MOVE(const QString &st_name, int st_count, QWidget *parent) :
    QDialog(parent),max_value(st_count),min_value(1),
    ui(new Ui::DIALOG_MOVE)
{
    ui->setupUi(this);
    ui->spinBox_pos->setMaximum(max_value);
    ui->spinBox_pos->setMinimum(min_value);
    ui->label_st_name->setText(tr("将")+st_name+tr("移入序列:"));
}

DIALOG_MOVE::~DIALOG_MOVE()
{
    delete ui;
}

void DIALOG_MOVE::on_pushButton_ok_clicked()
{
    emit SignalPos(ui->spinBox_pos->value());
}

void DIALOG_MOVE::on_pushButton_cancel_clicked()
{
    reject();
}
