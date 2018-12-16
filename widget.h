#ifndef WIDGET_H 
#define WIDGET_H

#include <QWidget>
#include <vector>
using std::vector;
#include "account.h"
#include "subway.h"

class LOGIN_DIALOG;
class DIALOG_CREAT_LINE;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class IN_USE_FAMILY;
class UN_USE_FAMILY;
enum FORMAT{NOR,ADM};//普通格式，管理格式

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
private:
    //私有数据
    SUBWAY subway;
    LOGIN_DIALOG*login_dialog;//管理员登陆窗口
    DIALOG_CREAT_LINE*dialog_creat_line;//新建线路窗口
    vector<ACCOUNT>admins;//负责存储管理员帐号密码
    ACCOUNT*cur_admin;//当前操作用户，非管理员用户时为0
    int line_obj_amount;//单行最多能容纳对象的数量
    int top_plan_amount;//换乘方案的最大显示数量
    QString admin_line;//正在被管理的线路
    vector<QObject*>no_del;//防勿删
    vector<QString>checked_st;//存储被checkedbox激活的站点
    int top;//线路查询排行数

    //辅助方法
    void BindFilters();//安装所有事件过滤器  complete
    void LabelCommonReaction(QLabel*label,QEvent*ev);//标签的通用事件反应  complete
    void ProtectControls();//将不期望删除的控件添加进no_del  complete
    void WriteHomePage(QWidget*w);//刷新主页面  complete
    void WriteLineHead(const QString&line_name, QVBoxLayout*w_vlayout, FORMAT format);//在垂直布局中写入path的第一个元素及其它相关信息  complete
    void WriteStations(PATH path, QVBoxLayout*w_vlayout, FORMAT format);//在垂直布局中写入PATH的站点信息  complete


    bool IsProtected(QObject*obj)const;//判断控件是否受到保护  complete
    void WhetherAvailable();//控制站点修改控件的激活  complete
    void RenewSwapBt();//更新交换站点控件的激活状态  complete
    void RenewMoveBt();//更新插入站点控件的激活状态  complete
    void RenewDelBt();//更新删除站点控件的激活状态  complete
    void RenewRenameBt();//更新重命名站点控件的激活状态

    void WriteTransferPlan(QWidget*w, const vector<PATH_MSG>&pms);//写入换乘页方案 complete
    QHBoxLayout* GetTrHeadLayout(int plans);//获取方案头的布局  complete
    QHBoxLayout* GetTrStartLayout(const QString&start);//获取起点站布局 complete
    QHBoxLayout* GetTrPassLayout(const PATH_MSG&pm,const QString&cur,const QString&next,const QString&line);//获取线路布局  complete
    QHBoxLayout* GetTrPointLayout(const QString&tr_point);//获取换乘站布局  complete
    QHBoxLayout* GetTrEndLayout(const QString&end);//获取终点站布局  complete
    QString GetStraightNext(const PATH_MSG&pm,const QString&cur);//获取最远直达站点名称  complete

    void WriteAdminHome(QWidget*w);//刷新管理主页面  complete
    void WriteLines(const vector<string>&lines, QWidget*page, MODE mode);//刷新管理主页面的某一个page，服务于WriteAdminHome方法  complete
    QWidget* CreatLineButtonFamily(const QString&line_name, MODE mode);//为管理页面新建线路管理button簇，并返回容纳它们的QHBoxlayout指针  complete
    void SetInuseFamily(IN_USE_FAMILY*family);//设置运行线路button簇  complete
    void SetUnuseFamily(UN_USE_FAMILY*family);//设置未运行线路button簇  complete

    void Login();//登陆  complete
    void Logout();//注销  complete
    void GoToAdminCenter();//进入管理中心 complete

    void InitCurrentWidgets();//初始化页面的当前显示  complete

    //修改信息方法簇
    void PutIntoUse(const QString&line);//将线路投入使用，刷新页面  complete
    void PutIntoStop(const QString&line);//将线路暂停使用，刷新页面  complete
    void DeleteLine(const QString&line);//删除备用线路，刷新页面  complete
    void WriteLineInfo(const QString&line,QWidget*w,FORMAT format);//重写线路信息 complete
    bool RenameStation(STATION*p_st, const QString&rename, MODE mode);//试图对p_st指向的站点进行重命名，成功则返回true  complete

    //刷新同名站点页面辅助方法簇
    void WriteSameNameSt(const QString&st_name, QWidget*w);//重写同名站点页  complete
    void WriteSameNameHead(QVBoxLayout*vlayout);//刷新写入同名站点页的标题,参数在WriteSameNameSt内部获得  complete
    void WriteSameNameInfo(const QString&st_name,QVBoxLayout*vlayout);//刷新写入同名站点信息，参数在WriteSameNameSt内部获得  complete
    //刷新站点信息页面辅助方法簇
    void WriteStationInfo(const STATION*p_st,FORMAT format, QWidget*w);//重写站点信息  complete
    void WriteStTrInfo(const STATION*p_st,QVBoxLayout*vlayout);//写入站点换乘信息  complete
    void WriteStGateInfo(const STATION*p_st,QVBoxLayout*vlayout,FORMAT format);//写入站点出入口信息  complete
    void WriteStBusInfo(const STATION*p_st,QVBoxLayout*vlayout,FORMAT format);//写入站点公交换乘信息  complete
    //刷新方案页面辅助方法簇
    void WritePlan(const vector<PATH_MSG> &tr_top, const vector<PATH_MSG> &ps_top,QWidget*w);//重写方案页面

    //刷新方法簇
    void RefreshNorHome();//刷新用户主页(用户主页可见站点名称)  complete
    void RefreshAdmHome();//刷新管理主页  complete
    void RefreshLineInfo(const QString&line_name, FORMAT format);//刷新线路信息  complete
    void RefreshStationInfo(const STATION*p_st,FORMAT format);//刷新站点信息  complete
    void RefreshSameNameSt(const QString&st_name);//刷新同名站点页面  complete
    void RefreshPlan(const vector<PATH_MSG>&tr_top,const vector<PATH_MSG>&ps_top);//刷新方案页面  complete

    //重显方法簇
    void ReShowNorHome();//重显用户主页面  complete
    void ReShowAdmHome();//重显管理主页面  complete
    void ReShowLineInfo(const QString&line_name, FORMAT format);//重显线路信息页面  complete
    void ReShowSameNameSt(const QString&st_name);//重显同名站点页面 complete
    void ReShowStationInfo(const STATION*p_st,FORMAT format);//重显站点信息页面  complete
    void ReShowPlan(const vector<PATH_MSG>&tr_top,const vector<PATH_MSG>&ps_top);//重显方案页面  complete

    bool PSGTipsOnTrInq();//调出用户换乘查询提示，若无需提示，则返回false  complete

public:
    explicit Widget(QWidget *parent = 0);
    bool eventFilter(QObject *obj, QEvent *ev);
    void InstalCompleter();//安装Completer
    ~Widget();

private slots:
    bool CompareAcc(const ACCOUNT&acc);//比对帐号密码槽函数  complete
    void on_toolButton_add_line_clicked();//添加线路按钮槽函数  complete
    void on_toolButton_creat_line_clicked();//新建线路槽按钮函数  complete
    void CreatLineToUnUse(const QString&line);//创建一条空的LINE到un_use_lines族里  complete
    void CreatStation(const QString&st_name);//新建站点  complete
    void AcceptChecked(bool checked, const QString &st_name);//接收STT_ADM的信号  complete
    void SlotMoveStation(int pos);//将钩选站点移入到pos位置，并刷新显示，可抛出异常  complete

    void on_toolButton_back_clicked();//回退控件槽  complete

    void on_pushButton_newst_clicked();//新建站点控件槽  complete

    void on_pushButton_swap_clicked();//交换站点控件槽  complete

    void on_pushButton_move_clicked();//移动站点控件槽  complete

    void on_pushButton_del_clicked();//删除站点控件槽  complete

    void on_toolButton_search_clicked();//管理搜索控件槽  complete

    void on_toolButton_tr_inq_clicked();//用户换乘查询控件槽  complete

    void on_toolButton_st_inq_clicked();//用户站点查询控件槽  complete

    void on_pushButton_save_clicked();//将轨道信息写入info文件  complete

    void on_toolButton_quit_clicked();//退出管理页面控件槽  complete

    void on_pushButton_gohome_clicked();//返回用户主页面  complete

    void on_pushButton_rename_clicked();//重命名站点控件槽

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
