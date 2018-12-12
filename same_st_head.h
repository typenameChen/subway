#ifndef SAME_ST_HEAD_H
#define SAME_ST_HEAD_H

#include <QWidget>

namespace Ui {
class SAME_ST_HEAD;
}

class SAME_ST_HEAD : public QWidget
{
    Q_OBJECT

public:
    explicit SAME_ST_HEAD(QWidget *parent = 0);
    ~SAME_ST_HEAD();

private:
    Ui::SAME_ST_HEAD *ui;
};

#endif // SAME_ST_HEAD_H
