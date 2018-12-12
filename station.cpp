#include "station.h"
#include <algorithm>

#include "testhead.h"//test

STATION::STATION(const string &nm):name(nm){}//构造站点

bool STATION::IsBelongedTo(const string &line_name) const//判断是否属于指定线路
{
    return std::count(blt_line.begin(),blt_line.end(),line_name);
}

int STATION::FindNext(const STATION *p_st) const//找寻并返回p_next_sts成员中与p_st指向同一站点的指针下标，查找失败则返回-1
{
    int i=0;
    for(STATION*p:p_next_sts)
    {
        if(p==p_st)
            return i;
        i++;
    }
    return -1;
}

void STATION::UniqueSts()//清理p_next_sts中的重复指针
{
    std::sort(p_next_sts.begin(),p_next_sts.end());
    vector<STATION*>::iterator it=std::unique(p_next_sts.begin(),p_next_sts.end());
    p_next_sts.erase(it,p_next_sts.end());
}

bool STATION::DeleteBelongedLine(const std::string &line_name)//删除名为line_name的所属线路，删除失败时返回false
{
    for(unsigned int i=0;i<blt_line.size();i++)
    {
        if(line_name==blt_line[i])
        {
            blt_line.erase(blt_line.begin()+i);
            return true;
        }
    }
    return false;
}

bool STATION::Cut(STATION *p_st)//剪断调用对象与p_st指向的对象之间的连系，若p_st不属于邻站则返回false
{
    int i=FindNext(p_st);
    if(i!=-1)
    {
        p_next_sts.erase(p_next_sts.begin()+i);
        int j=p_st->FindNext(this);
        p_st->p_next_sts.erase(p_st->p_next_sts.begin()+j);
        return true;
    }
    return false;
}

bool STATION::Connect(STATION *p_st)//连接调用对象与p_st指向的对象，其中调用对象离遍历用起点站较近,若它们已连接则返回false
{
    int i=FindNext(p_st);
    if(i!=-1)
        return false;
    p_next_sts.push_back(p_st);
    p_st->p_next_sts.push_back(this);
    return true;
}

int STATION::NumOfSharedLines(const STATION *p_st) const//获取调用对象与p_st同属的线路数量
{
    int ct=0;
    for(const string&line_name:p_st->blt_line)
    {
        if(IsBelongedTo(line_name))
            ct++;
    }
    return ct;
}

vector<std::string> STATION::NameOfSharedLines(const STATION *p_st) const//获取调用对象与p_st同属的所有线路名
{
    vector<string>shared_lines;
    for(const string&line_name:p_st->blt_line)
    {
        if(IsBelongedTo(line_name))
            shared_lines.push_back(line_name);
    }
    return shared_lines;
}

std::string STATION::GateMsg() const//获取出入口信息
{
    return gate_msg;
}

void STATION::SetGateMsg(const std::string &msg)//设置出入口信息
{
    gate_msg=msg;
}

std::string STATION::BusMsg() const//附近公交换乘信息
{
    return bus_msg;
}

void STATION::SetBusMsg(const std::string &msg)//设置附近公交信息
{
    bus_msg=msg;
}

STATION::~STATION()
{

}

