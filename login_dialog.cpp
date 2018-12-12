//登录窗口
#include "login_dialog.h"
#include "account.h"
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
LOGIN_DIALOG::LOGIN_DIALOG(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("管理员登陆窗口"));
    QLabel*label_acc=new QLabel(this);
    line_acc=new QLineEdit(this);
    line_acc->setFixedSize(QSize(125,27));
    QLabel*label_pass=new QLabel(this);
    line_pass=new QLineEdit(this);
    line_pass->setFixedSize(QSize(125,27));
    line_pass->setEchoMode(QLineEdit::Password);

    label_acc->setText(tr("用户名："));
    label_pass->setText(tr("密码："));

    QHBoxLayout*hlayout_acc=new QHBoxLayout;
    hlayout_acc->addWidget(label_acc);
    hlayout_acc->addWidget(line_acc);

    QHBoxLayout*hlayout_pass=new QHBoxLayout;
    hlayout_pass->addWidget(label_pass);
    hlayout_pass->addWidget(line_pass);

    QVBoxLayout*vlayout=new QVBoxLayout(this);
    vlayout->addLayout(hlayout_acc);
    vlayout->addLayout(hlayout_pass);

    button_ok=new QPushButton(this);
    button_cancel=new QPushButton(this);
    button_ok->setText(tr("确认"));
    button_cancel->setText(tr("取消"));

    QHBoxLayout*hlayout_button=new QHBoxLayout;
    hlayout_button->addWidget(button_ok);
    hlayout_button->addWidget(button_cancel);

    vlayout->addLayout(hlayout_button);

    connect(button_ok,&QPushButton::clicked,this,&LOGIN_DIALOG::SendLoginSignal);
    connect(button_cancel,&QPushButton::clicked,this,&LOGIN_DIALOG::close);
}
LOGIN_DIALOG::~LOGIN_DIALOG()
{

}

void LOGIN_DIALOG::SendLoginSignal()
{
    ACCOUNT usr_acc(line_acc->text(),line_pass->text());
    emit LoginSignal(usr_acc);
}

