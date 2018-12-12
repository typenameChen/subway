#include "st_tr_info.h"
#include "ui_st_tr_info.h"
#include "tr_msg.h"
ST_TR_INFO::ST_TR_INFO(const QString &st_nm, QWidget *parent) :
    QWidget(parent),st_name(st_nm),
    ui(new Ui::ST_TR_INFO)
{
    ui->setupUi(this);
    ui->verticalLayout->setAlignment(Qt::AlignTop);
}

void ST_TR_INFO::PushMsg(const QString &line_nm)
{
    TR_MSG*tr_msg=new TR_MSG(st_name,line_nm);
    ui->verticalLayout->addWidget(tr_msg);
}

ST_TR_INFO::~ST_TR_INFO()
{
    delete ui;
}
