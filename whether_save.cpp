//站点信息保存提示框
#include "whether_save.h"
#include "ui_whether_save.h"

WHETHER_SAVE::WHETHER_SAVE(QWidget *parent,const QString&head,const QString&question) :
    QDialog(parent),
    ui(new Ui::WHETHER_SAVE)
{
    ui->setupUi(this);
    setWindowTitle(head);
    ui->label_question->setText(question);
}

WHETHER_SAVE::~WHETHER_SAVE()
{
    delete ui;
}

void WHETHER_SAVE::on_pushButton_save_clicked()
{
    emit SignalSave();
}

void WHETHER_SAVE::on_pushButton_dontsave_clicked()
{
    emit SignalDontSave();
}

void WHETHER_SAVE::on_pushButton_cancel_clicked()
{
    emit SignalCancel();
}
