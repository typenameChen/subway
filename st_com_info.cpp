//站点信息窗口
//负责显示及提供修改站点信息接口
#include "st_com_info.h"
#include "ui_st_com_info.h"
#include "info_modify.h"
ST_COM_INFO::ST_COM_INFO(const STATION *p_st, FORMAT format, QWidget *parent) :
    QWidget(parent),p_station(p_st),
    ui(new Ui::ST_COM_INFO)
{
    ui->setupUi(this);
    ui->horizontalLayout->setAlignment(Qt::AlignBottom);
    if(format==NOR)
        ui->pushButton_modify->hide();
}

void ST_COM_INFO::SetHead(const QString &head)//设置标题
{
    ui->label_head->setText(head);
}

void ST_COM_INFO::SetMsg(const QString &msg)//设置内容
{
    ui->label__msg->setText(msg);
}

ST_COM_INFO::~ST_COM_INFO()
{
    delete ui;
}

void ST_COM_INFO::on_pushButton_modify_clicked()
{
    INFO_MODIFY*info_modify=new INFO_MODIFY(p_station,ui->label_head->text(),ui->label__msg->text());
    connect(info_modify,&INFO_MODIFY::SignalSave,this,&ST_COM_INFO::SlotSendModify);
    info_modify->exec();
}

void ST_COM_INFO::SlotSendModify(const QString &mdf_msg)
{
    emit SignalModify(mdf_msg);
}
