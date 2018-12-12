//站点重命名操作窗口
#ifndef RENAME_ST_H
#define RENAME_ST_H

#include <QDialog>

namespace Ui {
class RENAME_ST;
}
class STATION;
class RENAME_ST : public QDialog
{
    Q_OBJECT

public:
    explicit RENAME_ST(const STATION*p_st, QWidget *parent = 0);
    ~RENAME_ST();
signals:
    void SignalRename(const QString&rename);
private:
    Ui::RENAME_ST *ui;
};

#endif // RENAME_ST_H
