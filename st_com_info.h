//站点信息窗口  负责显示及提供修改站点信息接口
#ifndef ST_COM_INFO_H
#define ST_COM_INFO_H

#include <QWidget>
#include "widget.h"
namespace Ui {
class ST_COM_INFO;
}

class ST_COM_INFO : public QWidget
{
    Q_OBJECT
private:
    const STATION*p_station;
public:
    explicit ST_COM_INFO(const STATION*p_st,FORMAT format,QWidget *parent = 0);
    void SetHead(const QString&head);//设置标题 complete
    void SetMsg(const QString&msg);//设置内容  complete
    ~ST_COM_INFO();

signals:
    void SignalModify(const QString&mdf_msg);
private slots:
    void on_pushButton_modify_clicked();
    void SlotSendModify(const QString&mdf_msg);

private:
    Ui::ST_COM_INFO *ui;
};

#endif // ST_COM_INFO_H
