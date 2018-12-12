//站点换乘信息面板
#ifndef ST_TR_INFO_H
#define ST_TR_INFO_H

#include <QWidget>

namespace Ui {
class ST_TR_INFO;
}

class ST_TR_INFO : public QWidget
{
    Q_OBJECT
private:
    QString st_name;
public:
    explicit ST_TR_INFO(const QString&st_nm, QWidget *parent = 0);
    void PushMsg(const QString&line_nm);
    ~ST_TR_INFO();

private:
    Ui::ST_TR_INFO *ui;
};

#endif // ST_TR_INFO_H
