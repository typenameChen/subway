//站点重命名操作窗口
#include "rename_st.h"
#include "ui_rename_st.h"
#include <QMessageBox>
#include "station.h"
#include <QDebug>
RENAME_ST::RENAME_ST(const STATION*p_st, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RENAME_ST)
{
    ui->setupUi(this);
    if(p_st->blt_line.size()==1)
        ui->label_warning->hide();
    ui->label_rename->setText(tr("将")+tr(p_st->name.c_str())+tr("站点重命名为:"));
    connect(ui->pushButton_ok,&QPushButton::clicked,[this]()
    {
        if(ui->lineEdit_rename->text().isEmpty())
            QMessageBox::critical(this,tr(""),tr("请输入新的站点名！"));
        else
            emit SignalRename(ui->lineEdit_rename->text());
    });
    connect(ui->pushButton_cancel,&QPushButton::clicked,this,&RENAME_ST::reject);
}

RENAME_ST::~RENAME_ST()
{
    delete ui;
}
