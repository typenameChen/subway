//查询结果显示窗口
#ifndef PLAN_WIDGETS_H
#define PLAN_WIDGETS_H

#include <QWidget>

namespace Ui {
class PLAN_WIDGETS;
}

class PLAN_WIDGETS : public QWidget
{
    Q_OBJECT

public:
    explicit PLAN_WIDGETS(const QString&start,const QString&end, QWidget *parent = 0);
    QWidget* TopTrWidget();
    QWidget *TopPsWidget();
    ~PLAN_WIDGETS();

private:
    Ui::PLAN_WIDGETS *ui;
};

#endif // PLAN_WIDGETS_H
