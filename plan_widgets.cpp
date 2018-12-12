//查询结果显示窗口
#include "plan_widgets.h"
#include "ui_plan_widgets.h"

PLAN_WIDGETS::PLAN_WIDGETS(const QString &start, const QString &end, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PLAN_WIDGETS)
{
    ui->setupUi(this);
    ui->horizontalLayout_2->setAlignment(Qt::AlignLeft);
    ui->label_start->setText(start);
    ui->label_end->setText(end);
}

QWidget *PLAN_WIDGETS::TopTrWidget()
{
    return ui->widget_top_tr;
}

QWidget *PLAN_WIDGETS::TopPsWidget()
{
    return ui->widget_top_ps;
}

PLAN_WIDGETS::~PLAN_WIDGETS()
{
    delete ui;
}
