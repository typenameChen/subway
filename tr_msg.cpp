//换乘提示信息容器
#include "tr_msg.h"
#include "ui_tr_msg.h"

TR_MSG::TR_MSG(const QString &st_nm, const QString &line_nm, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TR_MSG)
{
    ui->setupUi(this);
    ui->label_station->setText(st_nm);
    ui->label_line->setText(line_nm);
}

TR_MSG::~TR_MSG()
{
    delete ui;
}
