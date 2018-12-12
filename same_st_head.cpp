//同名站点头信息容器
#include "same_st_head.h"
#include "ui_same_st_head.h"

SAME_ST_HEAD::SAME_ST_HEAD(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SAME_ST_HEAD)
{
    ui->setupUi(this);
}

SAME_ST_HEAD::~SAME_ST_HEAD()
{
    delete ui;
}
