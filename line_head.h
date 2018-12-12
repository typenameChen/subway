//线路头信息类  负责显示线路头信息
#ifndef LINE_HEAD_H
#define LINE_HEAD_H

#include <QWidget>
#include "widget.h"
namespace Ui {
class LINE_HEAD;
}

class LINE_HEAD : public QWidget
{
    Q_OBJECT

public:
    explicit LINE_HEAD(const QString&line_name, const SUBWAY&subway,FORMAT format,QWidget *parent = 0);
    ~LINE_HEAD();

private:
    Ui::LINE_HEAD *ui;
};

#endif // LINE_HEAD_H
