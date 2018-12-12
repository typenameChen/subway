#include "stt_adm.h"
#include "ui_stt_adm.h"
#include "subway.h"

STT_ADM::STT_ADM(const QString &st_name, const SUBWAY &subway, QWidget *parent) :
    QWidget(parent),p_st(nullptr),
    ui(new Ui::STT_ADM)
{
    ui->setupUi(this);
    ui->verticalLayout->setAlignment(Qt::AlignHCenter);
    if((p_st=subway.GetStation(st_name.toStdString(),IN_USE)))
    {
        ui->pushButton_name->setText(st_name);
        if(p_st->blt_line.size()>=2)
            ui->label_icon->setPixmap(QPixmap("://icons/refresh.png"));
    }
    else if((p_st=subway.GetStation(st_name.toStdString(),UN_USE)))
        ui->pushButton_name->setText(st_name);
    else
        ui->pushButton_name->setText("ERROR");

    connect(ui->pushButton_name,&QPushButton::clicked,this,&STT_ADM::SlotStationAddr);
}

STT_ADM::~STT_ADM()
{
    delete ui;
}

void STT_ADM::on_checkBox_clicked(bool checked)
{
    emit SignalCheck(checked,ui->pushButton_name->text());
}

void STT_ADM::SlotStationAddr()
{
    emit SignalView(p_st);
}
