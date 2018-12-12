//线路增加操作窗口
//负责传递新增线路名
#include "dialog_add_line.h"
#include "ui_dialog_add_line.h"

DIALOG_CREAT_LINE::DIALOG_CREAT_LINE(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DIALOG_ADD_LINE)
{
    ui->setupUi(this);
}

void DIALOG_CREAT_LINE::SetLabelText(const QString &msg)const
{
    ui->label->setText(msg);
}

DIALOG_CREAT_LINE::~DIALOG_CREAT_LINE()
{
    delete ui;
}

void DIALOG_CREAT_LINE::on_pushButton_2_clicked()
{
    reject();
}

void DIALOG_CREAT_LINE::on_pushButton_clicked()
{
    emit SignalGetLineName(ui->lineEdit->text());
}
