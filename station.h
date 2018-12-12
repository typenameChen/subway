//站点类  负责切断、连接与其他站点的关联，并返回与线路相关联的信息
#ifndef STATION_H
#define STATION_H
#include <string>
#include <vector>
using std::string;
using std::vector;

class STATION
{
private:
    string gate_msg;//出入口信息
    string bus_msg;//附近公交换乘信息
    STATION(const STATION&other);//隐藏复制构造函数
public:
    string name;//站点名
    vector<string>blt_line;//所属线路名
    vector<STATION*>p_next_sts;//所有指向邻站站点的指针
    STATION(const string&nm);//构造站点  complete
    bool IsBelongedTo(const string&line_name)const;//判断是否属于指定线路  complete
    int FindNext(const STATION*p_st)const;//找寻并返回p_next_sts成员中与p_st指向同一站点的指针下标，查找失败则返回-1  complete
    void UniqueSts();//清理p_next_sts中的重复指针
    bool DeleteBelongedLine(const string&line_name);//删除名为line_name的所属线路，删除失败时返回false  complete
    bool Cut(STATION*p_st);//剪断调用对象与p_st指向的对象之间的连系，若p_st不属于邻站则返回false  complete
    bool Connect(STATION*p_st);//连接调用对象与p_st指向的对象，其中调用对象离遍历用起点站较近,若它们已连接则返回false  complete
    int NumOfSharedLines(const STATION*p_st)const;//获取调用对象与p_st同属的线路数量  complete
    vector<string> NameOfSharedLines(const STATION*p_st)const;//获取调用对象与p_st同属的所有线路名  complete
    string GateMsg()const;//获取出入口信息  complete
    void SetGateMsg(const string&msg);//设置出入口信息  complete
    string BusMsg()const;//附近公交换乘信息  complete
    void SetBusMsg(const string&msg);//设置附近公交信息
    ~STATION();
};

#endif // STATION_H
