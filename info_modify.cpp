//站点信息修改窗口
#include "info_modify.h"
#include "ui_info_modify.h"
#include <QMessageBox>
#include "station.h"
#include "whether_save.h"
#include <QDebug>
INFO_MODIFY::INFO_MODIFY(const STATION *p_st, const QString &head, const QString &msg, QWidget *parent) :
    QDialog(parent),p_station(p_st),
    ui(new Ui::INFO_MODIFY)
{
    ui->setupUi(this);
    setWindowTitle(head);
    ui->textEdit->setText(msg);
    ori_msg=msg;
}

INFO_MODIFY::~INFO_MODIFY()
{
    delete ui;
}

void INFO_MODIFY::on_pushButton_save_clicked()
{
    emit SignalSave(ui->textEdit->toPlainText());
}

void INFO_MODIFY::on_pushButton_quit_clicked()
{
    if(ori_msg!=ui->textEdit->toPlainText())
    {
        WHETHER_SAVE*whether_save=new WHETHER_SAVE(this,windowTitle(),tr("是否保存对")+tr(p_station->name.c_str())+tr("站点的修改?"));
        whether_save->setAttribute(Qt::WA_DeleteOnClose);
        connect(whether_save,&WHETHER_SAVE::SignalSave,[this,whether_save]()
        {
            emit SignalSave(ui->textEdit->toPlainText());
            whether_save->accept();
            this->accept();
        });
        connect(whether_save,&WHETHER_SAVE::SignalDontSave,[this,whether_save]()
        {
            whether_save->reject();
            this->reject();
        });
        connect(whether_save,&WHETHER_SAVE::SignalCancel,[whether_save]()
        {
            whether_save->reject();
        });
        whether_save->exec();
    }
    else
        reject();

}
