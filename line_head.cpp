#include "line_head.h"
#include "ui_line_head.h"

LINE_HEAD::LINE_HEAD(const QString &line_name, const SUBWAY &subway, FORMAT format, QWidget *parent):
    QWidget(parent),
    ui(new Ui::LINE_HEAD)
{
    ui->setupUi(this);
    MODE mode=subway.GetModeNullThrow(line_name.toStdString());
    if(mode==ERR)
        ui->label_name->setText("ERROR");
    else
    {
        ui->label_name->setText(line_name);
        int pass=subway.GetLine(line_name.toStdString(),mode).Counter();
        ui->label_pass->setText(tr("(全程%1站)").arg(pass));
        if(format==ADM&&mode==UN_USE)
            ui->label_use->setText(tr("未运行"));
    }
}

LINE_HEAD::~LINE_HEAD()
{
    delete ui;
}
