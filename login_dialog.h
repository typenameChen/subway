#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QWidget>
#include <QDialog>
class QPushButton;
class QLineEdit;
class ACCOUNT;

class LOGIN_DIALOG : public QDialog//登陆窗口
{
    Q_OBJECT
private:
    QPushButton*button_ok;//登录确认按钮
    QPushButton*button_cancel;//取消登录按钮
    QLineEdit*line_acc;//帐号行输入框
    QLineEdit*line_pass;//密码行输入框
public:
    explicit LOGIN_DIALOG(QWidget *parent = 0);
    ~LOGIN_DIALOG();

signals:
    void LoginSignal(const ACCOUNT&acc);//该信号传递用户及密码给主控件
public slots:
    void SendLoginSignal();//发射LoginSignal信号
};

#endif // LOGIN_DIALOG_H
