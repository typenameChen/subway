//单个站点管理容器
#ifndef STT_ADM_H
#define STT_ADM_H

#include <QWidget>
class STATION;
namespace Ui {
class STT_ADM;
}
class SUBWAY;
class STT_ADM : public QWidget
{
    Q_OBJECT
private:
    STATION*p_st;
public:
    explicit STT_ADM(const QString&st_name,const SUBWAY&subway,QWidget *parent = 0);
    ~STT_ADM();
signals:
    void SignalCheck(bool checked,const QString&st_name);
    void SignalView(const STATION*p_st);
private slots:

    void on_checkBox_clicked(bool checked);
    void SlotStationAddr();//站点按钮槽，其内部发射携带站点指针的信号

private:
    Ui::STT_ADM *ui;
};

#endif // STT_ADM_H
