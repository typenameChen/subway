//同名站点操作控件容器
#include "same_st_family.h"
#include "ui_same_st_family.h"
#include <QMessageBox>
SAME_ST_FAMILY::SAME_ST_FAMILY(const QString&li_name,const QString&st_name,MODE mode, QWidget *parent) :
    QWidget(parent),line_name(li_name),station_name(st_name),
    ui(new Ui::SAME_ST_FAMILY)
{
    ui->setupUi(this);
    QString mode_str;
    if(mode==IN_USE)
        mode_str=tr("(运行中)");
    else if(mode==UN_USE)
        mode_str=tr("(未运行)");

    ui->label_line_nm->setText(line_name+mode_str);
    ui->pushButton_st_nm->setText(st_name);

    connect(ui->pushButton_st_nm,&QPushButton::clicked,this,&SAME_ST_FAMILY::SlotButtonStNm);
    connect(ui->toolButton_manage,&QToolButton::clicked,this,&SAME_ST_FAMILY::SlotButtonStNm);
    connect(ui->toolButton_del,&QToolButton::clicked,this,&SAME_ST_FAMILY::SlotButtonStNm);
}

SAME_ST_FAMILY::~SAME_ST_FAMILY()
{
    delete ui;
}

void SAME_ST_FAMILY::SlotButtonStNm()
{
    if(sender()==ui->pushButton_st_nm||sender()==ui->toolButton_manage)
        emit SignalNames(line_name,station_name);
    else if(sender()==ui->toolButton_del)
    {
        int ret=QMessageBox::question(this,tr("警告"),tr("确定要删除")+line_name+tr("中的站点")+station_name+tr("吗?"));
        if(ret==QMessageBox::Yes)
            emit SignalDel(line_name,station_name);
    }
}
