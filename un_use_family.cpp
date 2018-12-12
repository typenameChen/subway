//未运行线路管理控件容器
#include "un_use_family.h"
#include "ui_un_use_family.h"

UN_USE_FAMILY::UN_USE_FAMILY(const QString &line_name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UN_USE_FAMILY)
{
    ui->setupUi(this);
    ui->pushButton_line->setText(line_name);
    ui->toolButton_manage->setText(tr("管理"));
    ui->toolButton_act->setText(tr("投入使用"));
    ui->toolButton_del->setText(tr("删除"));
}

QPushButton *UN_USE_FAMILY::GetBtLine()
{
    return ui->pushButton_line;
}

QToolButton *UN_USE_FAMILY::GetBtManage()
{
    return ui->toolButton_manage;
}

QToolButton *UN_USE_FAMILY::GetBtAct()
{
    return ui->toolButton_act;
}

QToolButton *UN_USE_FAMILY::GetBtDel()
{
    return ui->toolButton_del;
}

QString UN_USE_FAMILY::GetName() const
{
    return ui->pushButton_line->text();
}

UN_USE_FAMILY::~UN_USE_FAMILY()
{
    delete ui;
}
