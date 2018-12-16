#ifndef SW_DATABASE_H
#define SW_DATABASE_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <string>
#include <vector>
#include <list>
using std::string;
using std::vector;
using std::list;

class TAB_PAIR//备份与原表配对情况类
{
public:
    enum STATUS{ALL,ONLY_ORI,ONLY_BK,NONE};//完整配对、只有原表、只有备份表、全无
    string ori;//原表名
    string bk;//备份表名
    STATUS status;//配对情况
    TAB_PAIR();//complete
    TAB_PAIR(const string&ori_name,const string&bk_name,STATUS st);//complete
};


class SW_DATABASE//数据库管理员
{
private:
    enum GROUP{ORI,BK};//原始表、备份表
    QSqlDatabase db;
    QSqlQuery query;
    bool is_from_file;//是否由文件读取到数据库
    list<string> VectorToList(const vector<string>vr);//将vector复制转化为list  complete
    bool IsOri(const string&tab_name);//是否原表  complete
    bool IsBk(const string&tab_name);//是否备份表  complete
public:
    bool UseDatabase(const string&db_name);//选择数据库  complete
    vector<string> GetTableNames();//获取所有表名  complete
    vector<string> GetTableNames(GROUP group);//按组获取表名  complete
    int GetItemCount(const string&tb_name);//获取表格的单元数量  complete
    string GetValue(const string&tb_name,int pos);//获取表格单元的值,pos为0将获取第一个非id非表头的值  complete
    void Cover();//首先判断备份表或原表是否丢失，有丢失则抛出异常，否则备份表格覆盖原表格,注意本方法不选择数据库  complete
    void CreateTable(const string&tb_name);//创建表格  complete
    void PushItem(const string&tab_name,const string&value);//为名为tab_name的表格添加行,注意本方法不选择数据库  complete
    vector<TAB_PAIR> GetTablePairs();//将表格配对，注意之配对当前选择的数据库  complete
    bool IsFromFile()const;//判断是否由文件读取到数据库
    void SetFileToSql(bool from_file);//设置是否有文件读取到数据库
    SW_DATABASE();//登陆、打开数据库，默认使用InuseLines数据  complele
    ~SW_DATABASE();
};

#endif // SW_DATABASE_H
