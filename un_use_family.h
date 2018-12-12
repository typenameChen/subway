//未运行线路管理控件容器
#ifndef UN_USE_FAMILY_H
#define UN_USE_FAMILY_H

#include <QWidget>
class QPushButton;
class QToolButton;
namespace Ui {
class UN_USE_FAMILY;
}

class UN_USE_FAMILY : public QWidget
{
    Q_OBJECT

public:
    explicit UN_USE_FAMILY(const QString&line_name, QWidget *parent = 0);
    QPushButton* GetBtLine();
    QToolButton* GetBtManage();
    QToolButton* GetBtAct();
    QToolButton* GetBtDel();
    QString GetName()const;
    ~UN_USE_FAMILY();

private:
    Ui::UN_USE_FAMILY *ui;
};

#endif // UN_USE_FAMILY_H
