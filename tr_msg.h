//换乘提示信息容器
#ifndef TR_MSG_H
#define TR_MSG_H

#include <QWidget>

namespace Ui {
class TR_MSG;
}

class TR_MSG : public QWidget
{
    Q_OBJECT

public:
    explicit TR_MSG(const QString&st_nm,const QString&line_nm, QWidget *parent = 0);
    ~TR_MSG();

private:
    Ui::TR_MSG *ui;
};

#endif // TR_MSG_H
