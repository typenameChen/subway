#ifndef STT_NOR_H
#define STT_NOR_H

#include <QWidget>

namespace Ui {
class STT_NOR;
}

class SUBWAY;
class STATION;
class QLabel;
class STT_NOR : public QWidget
{
    Q_OBJECT
private:
    STATION*p_st;
    void EnterAndLeave(QLabel*label,QEvent*ev);//处理进入离开事件  complete
protected:
    bool eventFilter(QObject *obj, QEvent *ev);//事件过滤器，处理进入,离,点击事件
public:
    explicit STT_NOR(const QString&st_name,const SUBWAY&subway,QWidget *parent = 0);
    void HideArrow();
    ~STT_NOR();
signals:
    void SignalView(const STATION*p_st);
private:
    Ui::STT_NOR *ui;
};

#endif // STT_NOR_H
