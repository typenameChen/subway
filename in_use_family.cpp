//停用线路操作控件容器类
#include "in_use_family.h"
#include "ui_in_use_family.h"

IN_USE_FAMILY::IN_USE_FAMILY(const QString &line_name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IN_USE_FAMILY)
{
    ui->setupUi(this);
    ui->pushButton_line->setText(line_name);
    ui->toolButton_manage->setText(tr("管理"));
    ui->toolButton_stop->setText(tr("停用"));
}

QPushButton *IN_USE_FAMILY::GetBtLine()
{
    return ui->pushButton_line;
}

QToolButton *IN_USE_FAMILY::GetBtManage()
{
    return ui->toolButton_manage;
}

QToolButton *IN_USE_FAMILY::GetBtStop()
{
    return ui->toolButton_stop;
}

QString IN_USE_FAMILY::GetName() const
{
    return ui->pushButton_line->text();
}

IN_USE_FAMILY::~IN_USE_FAMILY()
{
    delete ui;
}
