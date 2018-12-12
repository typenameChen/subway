//停用线路操作控件容器类
#ifndef IN_USE_FAMILY_H
#define IN_USE_FAMILY_H

#include <QWidget>
class QPushButton;
class QToolButton;
namespace Ui {
class IN_USE_FAMILY;
}

class IN_USE_FAMILY : public QWidget
{
    Q_OBJECT

public:
    explicit IN_USE_FAMILY(const QString&line_name, QWidget *parent = 0);
    QPushButton* GetBtLine();
    QToolButton* GetBtManage();
    QToolButton* GetBtStop();
    QString GetName()const;
    ~IN_USE_FAMILY();

private:
    Ui::IN_USE_FAMILY *ui;
};

#endif // IN_USE_FAMILY_H
