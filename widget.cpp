#include "widget.h"
#include "ui_widget.h"
#include "login_dialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QVBoxLayout>
#include "dialog_add_line.h"
#include <QPushButton>
#include "stt_nor.h"
#include "stt_adm.h"
#include "line_head.h"
#include "in_use_family.h"
#include "un_use_family.h"
#include "dialog_move.h"
#include "find.h"
#include "same_st_head.h"
#include "same_st_family.h"
#include "st_tr_info.h"
#include "st_com_info.h"
#include "plan_widgets.h"
#include <QCompleter>
#include "rename_st.h"
Widget::Widget(QWidget *parent) :
    QWidget(parent),line_obj_amount(6),checked_st(0),top(5),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    admins.push_back(ACCOUNT("su_admin","123456"));
    cur_admin=nullptr;

    ui->label_login->setCursor(Qt::PointingHandCursor);

    BindFilters();

    WriteHomePage(ui->scrollAreaWidgetContents_subway_home);
    WriteAdminHome(ui->toolBox_lines);

    ui->label_arrow->setPixmap(QPixmap("://icons/left-right.png"));

    InstalCompleter();
    ui->lineEdit_input_start->setPlaceholderText(tr("请输入起点站"));
    ui->lineEdit_input_end->setPlaceholderText(tr("请输入终点站"));
    ui->lineEdit_input_st->setPlaceholderText(tr("请输入站点名称"));
    ui->lineEdit_search->setPlaceholderText(tr("请输入线路或站点名称"));

    InitCurrentWidgets();//初始化页面的当前显示

    ProtectControls();//保护不希望删除的控件

    WhetherAvailable();//刷新站点管理控件的激活状态
}

bool Widget::eventFilter(QObject *obj, QEvent *ev)
{
    if(QString(obj->metaObject()->className())==QString("QLabel"))//如果事件接收对象是QLabel类对象
    {
        QLabel*label=static_cast<QLabel*>(obj);
        LabelCommonReaction(label,ev);//QLabel的通用事件反应

        if(ev->type()==QEvent::MouseButtonRelease)//点击标签的事件反应
        {
            //不同标签的特异性点击事件反应
            if(label==ui->label_login)
            {
                if(ui->label_login->text()==tr("管理员入口"))
                    Login();
                else
                    ReShowAdmHome();
            }
            else if(label==ui->label_log_off&&ui->label_log_off->text()==tr("注销"))
                Logout();
        }
    }
    return false;
}

void Widget::InstalCompleter()
{
    vector<PATH>all_key=subway.GetAllUseStatitonNames();
    QStringList list;
    for(const PATH&path:all_key)
    {
        for(const string&key:path)
            list<<key.c_str();
    }
    QCompleter*c_all=new QCompleter(list,this);
    ui->lineEdit_search->setCompleter(c_all);

    list.clear();
    for(const PATH&path:all_key)
    {
        for(unsigned int i=1;i<path.size();i++)
            list<<path[i].c_str();
    }
    QCompleter*c_st=new QCompleter(list,this);
    ui->lineEdit_input_start->setCompleter(c_st);
    ui->lineEdit_input_end->setCompleter(c_st);
    ui->lineEdit_input_st->setCompleter(c_st);
}

void Widget::BindFilters()//安装所有事件过滤器
{
    ui->label_login->installEventFilter(this);
    ui->label_log_off->installEventFilter(this);
}

void Widget::LabelCommonReaction(QLabel *label, QEvent *ev)
{
    QFont font=label->font();
    if(ev->type()==QEvent::Enter)
    {
        font.setUnderline(true);
        label->setFont(font);
    }
    else if(ev->type()==QEvent::Leave)
    {
        font.setUnderline(false);
        label->setFont(font);
    }
}

void Widget::ProtectControls()//将不期望删除的空间添加进no_del
{
    no_del=vector<QObject*>{
            ui->pushButton_del,ui->pushButton_move,ui->pushButton_newst,ui->pushButton_save,ui->pushButton_swap,
            ui->toolButton_add_line,ui->toolButton_back,ui->toolButton_creat_line,ui->toolButton_quit,
            ui->toolButton_search,ui->toolButton_st_inq,ui->toolButton_tr_inq,ui->adm_line_btcont};
}

void Widget::WriteHomePage(QWidget *w)//刷新主页面
{
    for(QObject*obj:w->children())//删除所有子控件
        delete obj;
    vector<PATH>all_lines=subway.GetAllUseStatitonNames();
    QVBoxLayout*vlayout=new QVBoxLayout(w);//在主页面中创建垂直布局
    vlayout->setAlignment(Qt::AlignTop);
    for(PATH path:all_lines)
    {
        QFrame*child_w=new QFrame(w);//在主页面里添加子区域
        vlayout->addWidget(child_w);//将子区域添加进垂直布局中
        child_w->setAttribute(Qt::WA_DeleteOnClose);//关闭即删除
        QVBoxLayout*vlayout_cld=new QVBoxLayout(child_w);//在子区域中创建垂直布局

        QString line_name=tr(path[0].c_str());
        WriteLineHead(line_name,vlayout_cld,NOR);//在子区域的垂直布局中写入线路名作为开头
        path.erase(path.begin());
        WriteStations(path,vlayout_cld,NOR);//在子区域的垂直布局中写入站点信息
    }
}

void Widget::WriteLineHead(const QString &line_name, QVBoxLayout *w_vlayout,FORMAT format)//在垂直布局中写入线路名头
{
    LINE_HEAD*line_head=new LINE_HEAD(line_name,subway,format);
    w_vlayout->addWidget(line_head);
}

void Widget::WriteStations(PATH path, QVBoxLayout *w_vlayout, FORMAT format)//在垂直布局中写入PATH的站点信息
{
    QHBoxLayout*hlayout=new QHBoxLayout;
    hlayout->setAlignment(Qt::AlignLeft);
    w_vlayout->addLayout(hlayout);
    int s=0;
    for(unsigned int i=0;i<path.size();i++)
    {
        QString st_name=tr(path[i].c_str());
        s++;
        if(s>line_obj_amount)
        {
            hlayout=new QHBoxLayout;
            hlayout->setAlignment(Qt::AlignLeft);
            w_vlayout->addLayout(hlayout);
            s=1;
        }
        QWidget*stt=nullptr;
        if(format==NOR)
        {
            stt=new STT_NOR(st_name,subway);
            connect(dynamic_cast<STT_NOR*>(stt),&STT_NOR::SignalView,[this](const STATION*p_st)
            {
                ReShowStationInfo(p_st,NOR);
            });
            if(i==path.size()-1)
                dynamic_cast<STT_NOR*>(stt)->HideArrow();
        }
        else if(format==ADM)
        {
            stt=new STT_ADM(st_name,subway);
            connect(dynamic_cast<STT_ADM*>(stt),&STT_ADM::SignalCheck,this,&Widget::AcceptChecked);
            connect(dynamic_cast<STT_ADM*>(stt),&STT_ADM::SignalView,[this](const STATION*p_st)
            {
                ReShowStationInfo(p_st,ADM);
            });
        }
        hlayout->addWidget(stt);
//        QLabel*label_arrow=new QLabel;
//        label_arrow->setPixmap(QPixmap("://icons/left-right.png"));
//        hlayout->addWidget(label_arrow);
    }
}

bool Widget::IsProtected(QObject *obj) const
{
    for(QObject*w:no_del)
    {
        if(w==obj)
            return true;
    }
    return false;
}

void Widget::WhetherAvailable()//控制站点修改控件的激活
{
    RenewSwapBt();
    RenewMoveBt();
    RenewDelBt();
    RenewRenameBt();
}

void Widget::RenewSwapBt()//更新交换站点控件的激活状态
{
    if(checked_st.size()!=2)
        ui->pushButton_swap->setDisabled(true);
    else
        ui->pushButton_swap->setDisabled(false);
}

void Widget::RenewMoveBt()
{
    if(checked_st.size()==1)
        ui->pushButton_move->setDisabled(false);
    else
        ui->pushButton_move->setDisabled(true);
}

void Widget::RenewDelBt()
{
    if(checked_st.size())
        ui->pushButton_del->setDisabled(false);
    else
        ui->pushButton_del->setDisabled(true);
}

void Widget::RenewRenameBt()
{
    if(checked_st.size()==1)
        ui->pushButton_rename->setDisabled(false);
    else
        ui->pushButton_rename->setDisabled(true);
}

void Widget::WriteTransferPlan(QWidget *w, const vector<PATH_MSG>&pms)//写入换乘页方案
{
    QVBoxLayout*vlayout_glb=new QVBoxLayout(w);//创建全局垂直布局
    int plans=0;
    for(const PATH_MSG&pm:pms)
    {
        QFrame*frame_child=new QFrame;//创建方案子区域
        QVBoxLayout*vlayout_cld=new QVBoxLayout(frame_child);//创建子区域的垂直布局
        vlayout_cld->setAlignment(Qt::AlignTop);

        QString start=tr((pm.GetPath())[0].c_str());
        QString end=tr(pm.GetPath().back().c_str());

        QHBoxLayout*hlayout_head=GetTrHeadLayout(++plans);//获取方案头的布局
        vlayout_cld->addLayout(hlayout_head);

        QHBoxLayout*hlayout_start=GetTrStartLayout(start);//获取起点站布局
        vlayout_cld->addLayout(hlayout_start);

        QString next=GetStraightNext(pm,start);

        QString line=tr((pm.GetLinesOrder())[0].c_str());
        QHBoxLayout*hlayout_pass=GetTrPassLayout(pm,start,next,line);//获取线路布局
        vlayout_cld->addLayout(hlayout_pass);

        for(int i=0;i<pm.GetTransferTimes();i++)
        {
            QString tr_point=tr((pm.GetTransferPoint())[i].c_str());
            QHBoxLayout*hlayout_tr=GetTrPointLayout(tr_point);//获取换乘站布局
            vlayout_cld->addLayout(hlayout_tr);

            next=GetStraightNext(pm,tr_point);
            line=tr((pm.GetLinesOrder())[i+1].c_str());
            QHBoxLayout*hlayout_pass=GetTrPassLayout(pm,tr_point,next,line);//获取线路布局
            vlayout_cld->addLayout(hlayout_pass);
        }
        QHBoxLayout*hlayout_end=GetTrEndLayout(end);//获取终点站布局
        vlayout_cld->addLayout(hlayout_end);

        vlayout_glb->addWidget(frame_child);
    }
}

QHBoxLayout *Widget::GetTrHeadLayout(int plans)//获取方案头的布局
{
    QHBoxLayout*hlayout=new QHBoxLayout;
    QLabel*label_plan=new QLabel(tr("方案%1:").arg(plans));
    hlayout->addWidget(label_plan);

    return hlayout;
}

QHBoxLayout *Widget::GetTrStartLayout(const QString &start)//获取起点站布局
{
    QHBoxLayout*hlayout=new QHBoxLayout;
    QLabel*label_icon=new QLabel;
    label_icon->setPixmap(QPixmap("://icons/info.png"));
    QLabel*label_start=new QLabel(start);

    hlayout->addWidget(label_icon);
    hlayout->addWidget(label_start);
    hlayout->setAlignment(Qt::AlignLeft);

    return hlayout;
}

QHBoxLayout *Widget::GetTrPassLayout(const PATH_MSG &pm, const QString &cur, const QString &next, const QString &line)//获取线路布局
{
    QHBoxLayout*hlayout=new QHBoxLayout;
    QLabel*label_icon=new QLabel;
    label_icon->setPixmap(QPixmap("://icons/up-down.png"));
    label_icon->setFixedHeight(70);
    label_icon->setFixedWidth(25);
    label_icon->setScaledContents(true);
    QLabel*label_line=new QLabel(line);
    PATH path=pm.GetPath();
    int cur_pos=pm.GetPos(cur.toStdString());
    int next_pos=pm.GetPos(next.toStdString());
    QLabel*label_pass=new QLabel(tr("(%1站)").arg(next_pos-cur_pos));

    hlayout->addWidget(label_icon);
    hlayout->addWidget(label_line);
    hlayout->addWidget(label_pass);

    return hlayout;
}

QHBoxLayout *Widget::GetTrPointLayout(const QString &tr_point)//获取换乘站布局
{
    QHBoxLayout*hlayout=new QHBoxLayout;
    QLabel*label_icon=new QLabel;
    label_icon->setPixmap(QPixmap("://icons/refresh.png"));
    QLabel*label_trp=new QLabel(tr_point);

    hlayout->addWidget(label_icon);
    hlayout->addWidget(label_trp);
    hlayout->setAlignment(Qt::AlignLeft);

    return hlayout;
}

QHBoxLayout *Widget::GetTrEndLayout(const QString &end)//获取终点站布局
{
    QHBoxLayout*hlayout=new QHBoxLayout;
    QLabel*label_icon=new QLabel;
    label_icon->setPixmap(QPixmap("://icons/ok.png"));
    QLabel*label_end=new QLabel(end);

    hlayout->addWidget(label_icon);
    hlayout->addWidget(label_end);
    hlayout->setAlignment(Qt::AlignLeft);

    return hlayout;
}

QString Widget::GetStraightNext(const PATH_MSG &pm, const QString &cur)//获取最远直达站点名称
{
    vector<string>tr_points=pm.GetTransferPoint();
    for(const string&tr_p:tr_points)
    {
        if(pm.GetPos(tr_p)>pm.GetPos(cur.toStdString()))
            return tr(tr_p.c_str());
    }
    return tr(pm.GetPath().back().c_str());
}

void Widget::WriteAdminHome(QWidget *w)//刷新管理主页面
{
    QToolBox*toolbox=static_cast<QToolBox*>(w);
    vector<string>lines=subway.GetAllLineNames(IN_USE);
    WriteLines(lines,toolbox->widget(0),IN_USE);
    lines=subway.GetAllLineNames(UN_USE);
    WriteLines(lines,toolbox->widget(1),UN_USE);
}

void Widget::WriteLines(const vector<std::string> &lines, QWidget *page,MODE mode)//刷新管理主页面的某一个page，服务于WriteAdminHome方法
{
    for(QObject*obj:page->children())//删除子控件
    {
        if(obj!=ui->toolButton_creat_line&&obj!=ui->toolButton_add_line)
            delete obj;
    }
    QVBoxLayout*vlayout=new QVBoxLayout(page);
    QHBoxLayout*hlayout;
    int s=0;
    for(const string&nm:lines)
    {
        if(s%2==0)
        {
            hlayout=new QHBoxLayout;
            vlayout->addLayout(hlayout);
        }
        QWidget*family=CreatLineButtonFamily(tr(nm.c_str()),mode);
        hlayout->addWidget(family);
        s++;
    }
}

QWidget *Widget::CreatLineButtonFamily(const QString &line_name, MODE mode)//为管理页面新建线路管理button簇，并返回容纳它们的QHBoxlayout指针
{
    QWidget*family;
    if(mode==IN_USE)
    {
        family=new IN_USE_FAMILY(line_name);
        SetInuseFamily(dynamic_cast<IN_USE_FAMILY*>(family));
    }
    else if(mode==UN_USE)
    {
        family=new UN_USE_FAMILY(line_name);
        SetUnuseFamily(dynamic_cast<UN_USE_FAMILY*>(family));
    }
    return family;
}

void Widget::SetInuseFamily(IN_USE_FAMILY *family)//设置运行线路button簇
{
    QString line_name=family->GetName();
    connect(family->GetBtLine(),&QPushButton::clicked,[this,line_name]()
    {
        ReShowLineInfo(line_name,ADM);
        ui->toolButton_back->raise();
    });
    connect(family->GetBtManage(),&QToolButton::clicked,[this,line_name]()
    {
        ReShowLineInfo(line_name,ADM);
        ui->toolButton_back->raise();
    });
    connect(family->GetBtStop(),&QToolButton::clicked,[this,line_name]()
    {
       PutIntoStop(line_name);
    });
}

void Widget::SetUnuseFamily(UN_USE_FAMILY *family)//设置未运行线路button簇
{
    QString line_name=family->GetName();
    connect(family->GetBtLine(),&QPushButton::clicked,[this,line_name]()
    {
        ReShowLineInfo(line_name,ADM);
        ui->toolButton_back->raise();
    });
    connect(family->GetBtManage(),&QToolButton::clicked,[this,line_name]()
    {
        ReShowLineInfo(line_name,ADM);
        ui->toolButton_back->raise();
    });
    connect(family->GetBtAct(),&QToolButton::clicked,[this,line_name]()
    {
        PutIntoUse(line_name);
    });
    connect(family->GetBtDel(),&QToolButton::clicked,[this,line_name]()
    {
        int ret=QMessageBox::question(this,tr("警告"),tr("确认删除")+line_name+tr("吗?"),QMessageBox::Yes,QMessageBox::No);
        if(ret==QMessageBox::Yes)
        {
            DeleteLine(line_name);
        }
    });
}

void Widget::Login()//管理员登陆按钮槽函数
{
     login_dialog=new LOGIN_DIALOG(this);
     login_dialog->setAttribute(Qt::WA_DeleteOnClose);
     connect(login_dialog,&LOGIN_DIALOG::LoginSignal,this,&Widget::CompareAcc);
     login_dialog->exec();
}

void Widget::Logout()
{
    ReShowNorHome();
    ui->label_login->setText(tr("管理员入口"));
    ui->label_log_off->setText(tr(""));
    ui->label_welcome->setText(tr(""));
}

void Widget::GoToAdminCenter()//进入管理中心
{
    ui->stackedWidget_subway->setCurrentWidget(ui->page_admin);
    ui->stackedWidget_admin->setCurrentWidget(ui->page_lines);
}

void Widget::InitCurrentWidgets()//初始化页面的当前显示
{
    ui->stackedWidget_subway->setCurrentIndex(0);//test
    ui->stackedWidget_admin->setCurrentIndex(0);
    ui->stackedWidget_normal_inq->setCurrentIndex(0);
    ui->toolBox_lines->setCurrentIndex(0);
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::CompareAcc(const ACCOUNT &acc)//比对帐号密码槽函数
{
    for(auto&admin:admins)
    {
        if(acc==admin)
        {
            login_dialog->accept();
            cur_admin=&admin;
            ui->label_welcome->setText(tr("欢迎你，"));
            ui->label_log_off->setText(tr("注销"));
            ui->label_login->setText(acc.account);

            return true;
        }
    }

    QMessageBox::warning(this,tr("登陆错误！"),tr("用户名或密码错误！"));
    return false;
}


void Widget::on_toolButton_add_line_clicked()//添加线路按钮槽函数
{
    ui->toolBox_lines->setCurrentWidget(ui->page_unuse);
}

void Widget::on_toolButton_creat_line_clicked()//新建线路按钮槽函数
{
    dialog_creat_line=new DIALOG_CREAT_LINE;
    dialog_creat_line->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog_creat_line,&DIALOG_CREAT_LINE::SignalGetLineName,this,&Widget::CreatLineToUnUse);
    dialog_creat_line->exec();
}

void Widget::CreatLineToUnUse(const QString &line)//创建一条空的LINE到un_use_lines族里
{
    try
    {
        subway.CreatLineToUnUse(line.toStdString());
    }
    catch(const string&err_msg)//若抛出错误，则给出提示，并直接返回
    {
        QMessageBox::critical(this,tr("错误"),tr(err_msg.c_str()));
        return;
    }
    //若线路顺利新建
    ReShowAdmHome();//重显管理主页面
    ui->toolButton_creat_line->raise();//将toolButton_creat_line按钮置顶

    dialog_creat_line->accept();//若线路顺利新建则关闭dialog_creat_line窗口
}

void Widget::CreatStation(const QString &st_name)//新建站点
{
    try
    {
        subway.AddStation(st_name.toStdString(),admin_line.toStdString());
        ReShowLineInfo(admin_line,ADM);
        RefreshNorHome();
    }
    catch(string&err_msg)
    {
        QMessageBox::critical(this,tr("错误"),tr(err_msg.c_str()));
        return;
    }
    dialog_creat_line->accept();
}

void Widget::AcceptChecked(bool checked,const QString&st_name)//接收STT_ADM的信号
{
    if(checked)
        checked_st.push_back(st_name);
    else
    {
        for(unsigned int i=0;i<checked_st.size();i++)
        {
            if(st_name==checked_st[i])
                checked_st.erase(checked_st.begin()+i);
        }
    }
    WhetherAvailable();
}

void Widget::SlotMoveStation(int pos)//将钩选站点移入到pos位置，并刷新显示，可抛出异常
{
    try
    {
        string line_name=admin_line.toStdString();//获取线路名的string版本
        string st_name=checked_st[0].toStdString();//获取选中站点名的string版本
        MODE mode=subway.GetMode(line_name);//获取线路的运行状态
        if(mode==IN_USE)
            subway.MoveLineToUnUse(line_name);//若未运行状态则投入到未运行组
        LINE&line=subway.GetLine(line_name);//获取线路对象的引用
        line.DeleteStation(st_name);//删除站点
        STATION*p_st=new STATION(st_name);//构造新的同名站点
        line.InsertStation(p_st,pos-1);//插入站点
        if(mode==IN_USE)
            subway.MoveLineToInUse(line_name);//若线路原始运行状态，则还原
        checked_st.clear();//清空钩选站点数组
        dynamic_cast<DIALOG_MOVE*>(sender())->accept();//关闭发送信息的控件

        RefreshNorHome();//刷新用户主页
        RefreshLineInfo(admin_line,ADM);//刷新线路信息页
        WhetherAvailable();//更新站点管理控件的激活状态
    }
    catch(string&err_msg)
    {
        QMessageBox::critical(this,tr("错误"),tr(err_msg.c_str()));
    }
}

void Widget::PutIntoUse(const QString &line)//将线路投入使用
{
    try
    {
        subway.MoveLineToInUse(line.toStdString());
        WriteAdminHome(ui->toolBox_lines);//刷新管理页
        WriteHomePage(ui->scrollAreaWidgetContents_subway_home);//刷新主页
    }
    catch(string&err_msg)
    {
        QMessageBox::critical(this,tr("错误"),tr(err_msg.c_str()));
    }
}

void Widget::PutIntoStop(const QString &line)//将线路暂停使用
{
    try
    {
        subway.MoveLineToUnUse(line.toStdString());
        WriteAdminHome(ui->toolBox_lines);//刷新管理页
        WriteHomePage(ui->scrollAreaWidgetContents_subway_home);//刷新主页
    }
    catch(string&err_msg)
    {
        QMessageBox::critical(this,tr("错误"),tr(err_msg.c_str()));
    }
}

void Widget::DeleteLine(const QString &line)//删除备用线路，刷新页面
{
    try
    {
        subway.DeleteLine(line.toStdString(),UN_USE);
        WriteAdminHome(ui->toolBox_lines);
        WriteHomePage(ui->scrollAreaWidgetContents_subway_home);
    }
    catch(string&err_msg)
    {
        QMessageBox::critical(this,tr("错误"),tr(err_msg.c_str()));
    }
}

void Widget::WriteLineInfo(const QString &line, QWidget *w, FORMAT format)//刷新写入线路信息
{
    for(QObject*obj:w->children())//释放子控件
    {
        if(!IsProtected(obj))//除了受保护的子控件
            delete obj;
    }
    try
    {
        MODE mode=subway.GetMode(line.toStdString());
        PATH path=subway.GetLine(line.toStdString(),mode).GetPath();

        QVBoxLayout*vlayout=new QVBoxLayout(w);
        vlayout->setAlignment(Qt::AlignTop);
        WriteLineHead(line,vlayout,format);
        WriteStations(path,vlayout,format);
    }
    catch(string&err_msg)
    {
        QMessageBox::critical(this,tr("错误"),tr(err_msg.c_str()));
    }
}

bool Widget::RenameStation(STATION *p_st, const QString &rename,MODE mode)//试图对p_st指向的站点进行重命名，成功则返回true
{
    if(mode==IN_USE)
    {
        STATION*p_re=subway.GetStation(rename.toStdString(),mode);
        if(p_re)
        {
            QMessageBox critical(QMessageBox::Critical,tr(""),tr("输入的名称不能与现有有站点重复！"),QMessageBox::Yes);
            critical.exec();
            return false;
        }
    }
    p_st->name=rename.toStdString();
    return true;
}

void Widget::WriteSameNameSt(const QString &st_name, QWidget *w)//刷新同名站点页面
{
    for(QObject*obj:w->children())
    {
        if(!IsProtected(obj))
            delete obj;
    }

    QVBoxLayout*vlayout=new QVBoxLayout(w);
    vlayout->setAlignment(Qt::AlignTop);
    WriteSameNameHead(vlayout);
    WriteSameNameInfo(st_name, vlayout);
}

void Widget::WriteSameNameHead(QVBoxLayout *vlayout)//写入同名站点页的标题,参数在WriteSameNameSt内部获得
{
    SAME_ST_HEAD*head=new SAME_ST_HEAD;
    vlayout->addWidget(head);
}

void Widget::WriteSameNameInfo(const QString &st_name, QVBoxLayout *vlayout)//写入同名站点信息，参数在WriteSameNameSt内部获得
{
    vector<string>lines=subway.GetLineNamesFromStation(st_name.toStdString());
    for(const string&line_nm:lines)
    {
        QString line_name=tr(line_nm.c_str());
        MODE mode=subway.GetMode(line_nm);
        SAME_ST_FAMILY*family=new SAME_ST_FAMILY(line_name,st_name,mode);
        connect(family,&SAME_ST_FAMILY::SignalNames,[this](const QString&line_name,const QString&st_name)
        {
            STATION*p_st=subway.GetStation(line_name.toStdString(),st_name.toStdString());
            ReShowStationInfo(p_st,ADM);
        });
        connect(family,&SAME_ST_FAMILY::SignalDel,[this](const QString&line_name,const QString&st_name)
        {
            subway.DeleteStation(st_name.toStdString(),line_name.toStdString());
            STATION*p_st=subway.GetStation(st_name.toStdString());
            if(p_st->blt_line.size()==1)
                ReShowStationInfo(p_st,ADM);
            else
                ReShowSameNameSt(st_name);
        });
        vlayout->addWidget(family);
    }
}

void Widget::WriteStationInfo(const STATION *p_st, FORMAT format, QWidget *w)//刷新写入站点信息
{
    for(QObject*obj:w->children())
    {
        if(!IsProtected(obj))
            delete obj;
    }
    QVBoxLayout*vlayout=new QVBoxLayout(w);
    WriteStTrInfo(p_st,vlayout);
    WriteStGateInfo(p_st,vlayout,format);
    WriteStBusInfo(p_st,vlayout,format);
}

void Widget::WriteStTrInfo(const STATION*p_st, QVBoxLayout *vlayout)//写入站点换乘信息
{
    ST_TR_INFO*stti=new ST_TR_INFO(tr(p_st->name.c_str()));
    for(const string&line_name:p_st->blt_line)
        stti->PushMsg(tr(line_name.c_str()));
    vlayout->addWidget(stti);
}

void Widget::WriteStGateInfo(const STATION *p_st, QVBoxLayout *vlayout, FORMAT format)//写入站点出入口信息
{
    ST_COM_INFO*stci=new ST_COM_INFO(p_st,format);
    stci->SetHead(tr(p_st->name.c_str())+tr("地铁站出入口信息"));
    stci->SetMsg(tr(p_st->GateMsg().c_str()));
    connect(stci,&ST_COM_INFO::SignalModify,[p_st,format,this](const QString&msg)
    {
        const_cast<STATION*>(p_st)->SetGateMsg(msg.toStdString());
        RefreshStationInfo(p_st,format);
    });
    vlayout->addWidget(stci);
}

void Widget::WriteStBusInfo(const STATION *p_st, QVBoxLayout *vlayout, FORMAT format)//写入站点公交换乘信息
{
    ST_COM_INFO*stci=new ST_COM_INFO(p_st,format);
    stci->setAttribute(Qt::WA_DeleteOnClose);
    stci->SetHead(tr(p_st->name.c_str())+tr("地铁站公交换乘信息"));
    stci->SetMsg(tr(p_st->BusMsg().c_str()));
    connect(stci,&ST_COM_INFO::SignalModify,[p_st,format,this](const QString&msg)
    {
        const_cast<STATION*>(p_st)->SetBusMsg(msg.toStdString());
        RefreshStationInfo(p_st,format);
    });
    vlayout->addWidget(stci);
}

void Widget::WritePlan(const vector<PATH_MSG> &tr_top, const vector<PATH_MSG> &ps_top, QWidget *w)//重写方案页面
{
    for(QObject*obj:w->children())
    {
        if(!IsProtected(obj))
            delete obj;
    }
    QGridLayout*glayout=new QGridLayout(w);
    PATH_MSG pm=tr_top[0];
    PATH path=pm.GetPath();
    QString start=path[0].c_str();
    QString end=path.back().c_str();

    PLAN_WIDGETS*plan_widgets=new PLAN_WIDGETS(start,end);
    glayout->addWidget(plan_widgets);
    WriteTransferPlan(plan_widgets->TopTrWidget(),tr_top);
    WriteTransferPlan(plan_widgets->TopPsWidget(),ps_top);
}

void Widget::RefreshNorHome()//刷新用户主页(用户主页可见站点名称)
{
    WriteHomePage(ui->scrollAreaWidgetContents_subway_home);
}

void Widget::RefreshAdmHome()//刷新管理主页
{
    WriteAdminHome(ui->toolBox_lines);
}

void Widget::RefreshLineInfo(const QString &line_name, FORMAT format)//刷新线路信息
{
    checked_st.clear();
    WriteLineInfo(line_name,ui->scrollAreaWidgetContents_admin_line,format);
}

void Widget::RefreshStationInfo(const STATION *p_st, FORMAT format)//刷新站点信息
{
    if(format==NOR)
        WriteStationInfo(p_st,format,ui->scrollAreaWidgetContents_subway_st);
    else if(format==ADM)
        WriteStationInfo(p_st,format,ui->scrollAreaWidgetContents_admin_st);
}

void Widget::RefreshSameNameSt(const QString &st_name)//刷新同名站点页面
{
    WriteSameNameSt(st_name,ui->page_same_station);
}

void Widget::RefreshPlan(const vector<PATH_MSG> &tr_top, const vector<PATH_MSG> &ps_top)//刷新方案页面
{
    WritePlan(tr_top,ps_top,ui->scrollAreaWidgetContents_subway_tr);
}

void Widget::ReShowNorHome()
{
    RefreshNorHome();
    ui->tabWidget->setCurrentWidget(ui->tab_subway);
    ui->stackedWidget_subway->setCurrentWidget(ui->page_normal_inq);
    ui->stackedWidget_normal_inq->setCurrentWidget(ui->page_subway_home);
}

void Widget::ReShowAdmHome()//重显管理主页面
{
    RefreshAdmHome();
    ui->toolButton_back->setDisabled(true);
    ui->tabWidget->setCurrentWidget(ui->tab_subway);
    ui->stackedWidget_subway->setCurrentWidget(ui->page_admin);
    ui->stackedWidget_admin->setCurrentWidget(ui->page_lines);
    ui->stackedWidget_normal_inq->setCurrentWidget(ui->page_subway_home);
    ui->tabWidget_inquiry->setCurrentWidget(ui->tab_tr_inquiry);

}

void Widget::ReShowLineInfo(const QString &line_name, FORMAT format)//重显线路信息页面
{
    admin_line=line_name;
    RefreshLineInfo(line_name,format);
    ui->toolButton_back->setDisabled(false);
    ui->tabWidget->setCurrentWidget(ui->tab_subway);
    ui->stackedWidget_subway->setCurrentWidget(ui->page_admin);
    ui->stackedWidget_admin->setCurrentWidget(ui->page_admin_line);
}

void Widget::ReShowSameNameSt(const QString &st_name)//重显同名站点页面
{
    RefreshSameNameSt(st_name);
    ui->toolButton_back->setDisabled(false);
    ui->tabWidget->setCurrentWidget(ui->tab_subway);
    ui->stackedWidget_subway->setCurrentWidget(ui->page_admin);
    ui->stackedWidget_admin->setCurrentWidget(ui->page_same_station);
}

void Widget::ReShowStationInfo(const STATION*p_st, FORMAT format)//重显站点信息页面
{
    RefreshStationInfo(p_st,format);
    ui->toolButton_back->setDisabled(false);
    ui->tabWidget->setCurrentWidget(ui->tab_subway);
    if(format==NOR)
    {
        ui->stackedWidget_subway->setCurrentWidget(ui->page_normal_inq);
        ui->stackedWidget_normal_inq->setCurrentWidget(ui->page_subway_st);
    }
    else if(format==ADM)
    {
        ui->stackedWidget_subway->setCurrentWidget(ui->page_admin);
        ui->stackedWidget_admin->setCurrentWidget(ui->page_admin_station);
    }
}

void Widget::ReShowPlan(const vector<PATH_MSG> &tr_top, const vector<PATH_MSG> &ps_top)//重显方案页面
{
    RefreshPlan(tr_top,ps_top);
    ui->toolButton_back->setDisabled(false);
    ui->tabWidget->setCurrentWidget(ui->tab_subway);
    ui->stackedWidget_subway->setCurrentWidget(ui->page_normal_inq);
    ui->stackedWidget_normal_inq->setCurrentWidget(ui->page_subway_tr);
}

bool Widget::PSGTipsOnTrInq()//调出用户换乘查询提示，若无需提示，则返回false
{
    if(ui->lineEdit_input_start->text().isEmpty())
    {
        QMessageBox::information(this,tr("信息不完整"),tr("请输入起点站名称"));
        return true;
    }
    if(ui->lineEdit_input_end->text().isEmpty())
    {
        QMessageBox::information(this,tr("信息不完整"),tr("请输入终点站名称"));
        return true;
    }

    STATION*p_start=subway.GetStation(ui->lineEdit_input_start->text().toStdString(),IN_USE);
    if(!p_start)
    {
        QMessageBox::information(this,tr("信息错误"),tr("未找到站点:")+ui->lineEdit_input_start->text());
        return true;
    }
    STATION*p_end=subway.GetStation(ui->lineEdit_input_end->text().toStdString(),IN_USE);
    if(!p_end)
    {
        QMessageBox::information(this,tr("信息错误"),tr("未找到站点:")+ui->lineEdit_input_end->text());
        return true;
    }

    if(ui->lineEdit_input_start->text()==ui->lineEdit_input_end->text())
    {
        QMessageBox::information(this,tr(" "),tr("亲，您真可爱，所以我选择原谅您，相信这只是一次意外"));
        return true;
    }

    return false;
}

void Widget::on_toolButton_back_clicked()
{
    ReShowAdmHome();
   //ui->stackedWidget_admin->setCurrentIndex(0);
}

void Widget::on_pushButton_newst_clicked()//新建站点控件槽
{
    dialog_creat_line=new DIALOG_CREAT_LINE;
    dialog_creat_line->setAttribute(Qt::WA_DeleteOnClose);
    dialog_creat_line->SetLabelText(tr("请输入站点名称"));
    connect(dialog_creat_line,&DIALOG_CREAT_LINE::SignalGetLineName,this,&Widget::CreatStation);
    dialog_creat_line->exec();
}

void Widget::on_pushButton_swap_clicked()//交换站点控件槽
{
    try
    {
        MODE mode=subway.GetMode(admin_line.toStdString());//获取线路是否运行

        if(mode==IN_USE)
            subway.MoveLineToUnUse(admin_line.toStdString());//若为运行线路则将其投入未运行

        STATION* p_st1=subway.GetLine(admin_line.toStdString()).Find(checked_st[0].toStdString());//获取钩选站点指针
        STATION* p_st2=subway.GetLine(admin_line.toStdString()).Find(checked_st[1].toStdString());//获取候选站点指针

        string temp=p_st1->name;
        p_st1->name=p_st2->name;          //交换站点名
        p_st2->name=temp;
        if(mode==IN_USE)
            subway.MoveLineToInUse(admin_line.toStdString());//若线路原是运行线路，则还原

        RefreshNorHome();//刷新用户主页
        RefreshLineInfo(admin_line,ADM);//刷新线路信息页

        checked_st.clear();//清空钩选站点数组
        WhetherAvailable();//更新站点修改控件的激活状态
    }
    catch(string&err_msg)
    {
        QMessageBox::critical(this,tr("错误"),tr(err_msg.c_str()));
    }
}

void Widget::on_pushButton_move_clicked()//移动站点控件槽
{
    try
    {
        int count=subway.GetLine(admin_line.toStdString()).Counter();//获取线路的站点数
        DIALOG_MOVE*dialog=new DIALOG_MOVE(checked_st[0],count);//使用钩选站点名和线路站点数初始化dialog以获取必要信息
        connect(dialog,&DIALOG_MOVE::SignalPos,this,&Widget::SlotMoveStation);//SignalPos附带移入位置信息，SlotMoveStation负责移动站点，刷新显示，可抛出异常
        dialog->exec();
    }
    catch(string&err_msg)
    {
        QMessageBox::critical(this,tr("错误"),tr(err_msg.c_str()));
    }
}

void Widget::on_pushButton_del_clicked()//删除站点控件槽
{
    try
    {
        int ret=QMessageBox::question(this,tr("警告"),tr("确认删除所选%1个站点吗?").arg(checked_st.size()));//确认窗口
        if(ret==QMessageBox::Yes)
        {
            for(const QString&st_name:checked_st)
                subway.DeleteStation(st_name.toStdString(),admin_line.toStdString());//删除所有所选站点
        }
        checked_st.clear();//清空钩选站点数组
        WhetherAvailable();//更新站点管理控件激活状态
        RefreshNorHome();//刷新用户主页
        RefreshLineInfo(admin_line,ADM);//刷新线路信息页
    }
    catch(string&err_msg)
    {
        QMessageBox::critical(this,tr("错误"),tr(err_msg.c_str()));
    }
}

void Widget::on_toolButton_search_clicked()//管理搜索控件槽
{
    FIND find=subway.Search(ui->lineEdit_search->text().toStdString());//获取查找到的元素信息对象
    if(!find.Count())//如果什么都没找到
        QMessageBox::information(this,tr(""),tr("未找到相关信息"));//调出提示信息窗口
    if(find.Type()==string("LINE"))//如果找到的元素是线路
        ReShowLineInfo(ui->lineEdit_search->text(),ADM);//重显线路信息页面
    else if(find.Type()==string("STATION"))//如果找到的元素是站点
    {
        if(find.Count()>1)//如果存在同名站点
            ReShowSameNameSt(ui->lineEdit_search->text());//重显同名站点页面
        else
        {
            const STATION*p_st=subway.GetStation(ui->lineEdit_search->text().toStdString());
            ReShowStationInfo(p_st,ADM);//否则重显站点信息页面
        }
    }
    ui->lineEdit_search->clear();//清除搜索框内容
}

void Widget::on_toolButton_tr_inq_clicked()//用户换乘查询控件槽
{
    if(PSGTipsOnTrInq())//乘客查询导致提示窗口被调出
        return;//则直接返回

    string start=ui->lineEdit_input_start->text().toStdString();//获取起点站名的string版本
    string end=ui->lineEdit_input_end->text().toStdString();//获取终点站名的string版本
    vector<PATH_MSG>tr_top=subway.GetTopPaths(start,end,top,TRANSFER);//获取换乘排行方案
    vector<PATH_MSG>ps_top=subway.GetTopPaths(start,end,top,PASS);//获取经站排行方案
    ReShowPlan(tr_top,ps_top);//重显方案页面
}

void Widget::on_toolButton_st_inq_clicked()
{
    string st_name=ui->lineEdit_input_st->text().toStdString();
    STATION*p_st=subway.GetStation(st_name,IN_USE);
    if(!p_st)
    {
        QMessageBox::information(this,tr("信息错误"),tr("未找到站点:")+ui->lineEdit_input_st->text());
        return;
    }
    ReShowStationInfo(p_st,NOR);
}

void Widget::on_pushButton_save_clicked()
{
    subway.WriteFile();
}

void Widget::on_toolButton_quit_clicked()
{
    ReShowNorHome();
}

void Widget::on_pushButton_gohome_clicked()
{
    ReShowNorHome();
}

void Widget::on_pushButton_rename_clicked()
{
    STATION*p_st=subway.GetStation(admin_line.toStdString(),checked_st[0].toStdString());//获取待重命名的站点指针
    RENAME_ST*rename_st=new RENAME_ST(p_st);//创建重命名窗口
    rename_st->setAttribute(Qt::WA_DeleteOnClose);     
    MODE mode=subway.GetMode(admin_line.toStdString());
    connect(rename_st,&RENAME_ST::SignalRename,[this,p_st,rename_st,mode](const QString&rename)//重命名窗口可发送重命名信号，其携带新的名称
    {
        if(RenameStation(p_st,rename,mode))//若重命名成功
        {
            ReShowLineInfo(admin_line,ADM);//则重显线路信息窗口
            rename_st->accept();//则关闭重命名窗口
        }
    });
    rename_st->exec();
}
