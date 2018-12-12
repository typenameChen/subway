#include "stt_nor.h"
#include "ui_stt_nor.h"
#include "subway.h"
void STT_NOR::EnterAndLeave(QLabel *label, QEvent *ev)//处理进入离开事件
{
    QFont font=label->font();
    if(ev->type()==QEvent::Enter)
    {
        font.setUnderline(true);
        label->setFont(font);
    }
    else if(ev->type()==QEvent::Leave)
    {
        font.setUnderline(false);
        label->setFont(font);
    }
}

bool STT_NOR::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj==ui->label_name)
    {
        EnterAndLeave(ui->label_name,ev);
        if(ev->type()==QEvent::MouseButtonRelease)
        {
            emit SignalView(p_st);
        }
    }
    return false;
}

STT_NOR::STT_NOR(const QString &st_name,const SUBWAY&subway, QWidget *parent) :
    QWidget(parent),p_st(nullptr),
    ui(new Ui::STT_NOR)
{
    ui->setupUi(this);
    //ui->horizontalLayout->setAlignment(Qt::AlignBottom);
    if((p_st=subway.GetStation(st_name.toStdString()),IN_USE))
    {
        ui->label_name->setText(st_name);
        if(p_st->blt_line.size()>=2)
            ui->label_icon->setPixmap(QPixmap("://icons/refresh.png"));
    }
    else if((p_st=subway.GetStation(st_name.toStdString()),UN_USE))
        ui->label_name->setText(st_name);
    else
        ui->label_name->setText("ERROR");

    ui->label_name->installEventFilter(this);
    ui->label_name->setCursor(Qt::PointingHandCursor);
}

void STT_NOR::HideArrow()
{
    ui->label_arrow->hide();
}

STT_NOR::~STT_NOR()
{
    delete ui;
}
