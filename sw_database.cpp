#include "sw_database.h"
#include <QSqlError>
#include <QVariant>
#include <algorithm>
#include <QDebug>
using std::find;
bool SW_DATABASE::UseDatabase(const string &db_name)//选择数据库
{
    if(!query.exec(QString("use ")+db_name.c_str()))
        throw query.lastError().text().toStdString();
    return true;
}

vector<std::string> SW_DATABASE::GetTableNames()//获取所有表名
{
    vector<string>tb_names;
    query.exec("SHOW TABLES");
    while(query.next())
        tb_names.push_back(query.value(0).toString().toStdString());
    return tb_names;
}

vector<std::string> SW_DATABASE::GetTableNames(SW_DATABASE::GROUP group)//按组获取表名
{
    vector<string>all_tabs=GetTableNames();
    vector<string>group_tabs;
    for(const string&tab_name:all_tabs)
    {
        if((group==ORI&&IsOri(tab_name))||(group==BK&&IsBk(tab_name)))
            group_tabs.push_back(tab_name);
    }
    return group_tabs;
}

int SW_DATABASE::GetItemCount(const std::string &tb_name)//获取表格的单元数量
{
    query.exec(QString("SELECT*FROM ")+tb_name.c_str());
    int c=0;
    while(query.next())++c;
    return c;
}

std::string SW_DATABASE::GetValue(const std::string &tb_name, int pos)//获取表格单元的值
{
    int p=0;
    query.exec(QString("SELECT station_name FROM ")+tb_name.c_str());
    while(query.next())
    {
        if(p==pos)
            return query.value(0).toString().toStdString();
        p++;
    }

    throw QString("SW_DATABASE::GetValue:试图在表%1的无效位置pos:%2获取站点").arg(tb_name.c_str()).arg(pos).toStdString();
}

void SW_DATABASE::Cover()//首先判断更新表或原表是否丢失，有丢失则抛出异常，否则更新表格覆盖原表格
{
    vector<TAB_PAIR> tab_pairs=GetTablePairs();
    //表格配对失败则抛出异常
    for(const TAB_PAIR&tp:tab_pairs)
    {
        if(tp.status!=TAB_PAIR::ALL)
        {
            string prefix="SW_DATABASE::Cover：数据库中缺少";
            string suffix="的更新表格";
            if(tp.status==TAB_PAIR::ONLY_ORI)
                throw prefix+tp.ori+suffix;
            else if(tp.status==TAB_PAIR::ONLY_BK)
            {
                if(IsFromFile())
                    const_cast<TAB_PAIR&>(tp).ori=tp.bk.substr(0,tp.bk.size()-3);
                else
                    throw prefix+tp.bk+suffix;
            }
        }
    }
    //更新表覆盖原表
    for(const TAB_PAIR&tp:tab_pairs)
    {
        query.exec(QString("DROP TABLE ")+tp.ori.c_str());
        query.exec(QString("ALTER TABLE ")+tp.bk.c_str()+QString(" RENAME ")+tp.ori.c_str());
    }
}

list<std::string> SW_DATABASE::VectorToList(const vector<std::string> vr)//将vector复制转化为list
{
    list<string>lr;
    for(const string&str:vr)
        lr.push_back(str);
    return lr;
}

bool SW_DATABASE::IsOri(const std::string &tab_name)//是否为原表
{
    string dist_part=tab_name.substr(tab_name.size()-3,tab_name.size());
    return dist_part!=string("_bk");
}

bool SW_DATABASE::IsBk(const std::string &tab_name)//是否为更新表
{
    return !IsOri(tab_name);
}

void SW_DATABASE::CreateTable(const std::string &tb_name)//创建表格
{
    query.exec(QString("CREATE TABLE ")+tb_name.c_str()+" (station_id INT PRIMARY KEY NOT NULL AUTO_INCREMENT,"
                                                        "station_name TEXT NOT NULL)");
}

void SW_DATABASE::PushItem(const std::string &tab_name, const std::string &value)//为名为tab_name的表格添加行,注意本方法不选择数据库
{
    query.exec(QString("INSERT INTO ")+tab_name.c_str()+" VALUES (NULL,"+"\'"+value.c_str()+"\'"+")");
}

vector<TAB_PAIR> SW_DATABASE::GetTablePairs()//将表格配对，注意只配对当前选择的数据库
{
    list<string>tab_oris=VectorToList(GetTableNames(ORI));
    list<string>tab_bks=VectorToList(GetTableNames(BK));
    vector<TAB_PAIR>tab_pairs;
    while(tab_oris.size())
    {
        string fst_ori=*tab_oris.begin();
        list<string>::iterator it=find(tab_bks.begin(),tab_bks.end(),fst_ori+"_bk");
        if(it!=tab_bks.end())
        {
            tab_pairs.push_back(TAB_PAIR(fst_ori,*it,TAB_PAIR::ALL));
            tab_oris.pop_front();
            tab_bks.erase(it);
        }
        else
        {
            tab_pairs.push_back(TAB_PAIR(fst_ori,"",TAB_PAIR::ONLY_ORI));
            tab_oris.pop_front();
        }
    }
    for(const string&bk_name:tab_bks)
        tab_pairs.push_back(TAB_PAIR("",bk_name,TAB_PAIR::ONLY_BK));
    return tab_pairs;
}

bool SW_DATABASE::IsFromFile() const//判断是否由文件读取到数据库
{
    return is_from_file;
}

void SW_DATABASE::SetFileToSql(bool from_file)//设置是否由文件读取到数据库
{
    is_from_file=from_file;
}

SW_DATABASE::SW_DATABASE(const string&username,const string&password,const string&use_db1, const string&use_db2)//登陆与设置数据库，默认使用use_db1数据库
{
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setUserName(username.c_str());
    db.setPassword(password.c_str());
//    db.setDatabaseName(use_db1.c_str());
    query=QSqlQuery(db);
    if(!db.open())
        throw db.lastError().text().toStdString();
    is_from_file=false;

    if(!query.exec(QString("USE %1").arg(use_db1.c_str())))
        query.exec(QString("CREATE DATABASE ")+use_db1.c_str());
    if(!query.exec(QString("USE %1").arg(use_db2.c_str())))
        query.exec(QString("CREATE DATABASE ")+use_db2.c_str());

    query.exec(QString("USE %1").arg(use_db1.c_str()));
}

SW_DATABASE::~SW_DATABASE()
{

}



TAB_PAIR::TAB_PAIR():status(NONE)
{

}

TAB_PAIR::TAB_PAIR(const std::string &ori_name, const std::string &bk_name, TAB_PAIR::STATUS st):ori(ori_name),bk(bk_name),status(st)
{

}
