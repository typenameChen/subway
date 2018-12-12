//同名站点操作控件容器  当管理员查询到同名站点时，可通过此容器做进一步操作
#ifndef SAME_ST_FAMILY_H
#define SAME_ST_FAMILY_H

#include <QWidget>
#include "subway.h"
namespace Ui {
class SAME_ST_FAMILY;
}

class SAME_ST_FAMILY : public QWidget
{
    Q_OBJECT
private:
    QString line_name;
    QString station_name;
public:
    explicit SAME_ST_FAMILY(const QString&line_name,const QString&st_name,MODE mode, QWidget *parent = 0);
    ~SAME_ST_FAMILY();

private:
    Ui::SAME_ST_FAMILY *ui;
signals:
    void SignalNames(const QString&line_name,const QString&st_name);
    void SignalDel(const QString&line_name,const QString&st_name);
private slots:
    void SlotButtonStNm();
};

#endif // SAME_ST_FAMILY_H
