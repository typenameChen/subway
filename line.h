#ifndef LINE_H
#define LINE_H
#include <string>
#include <vector>

#include "testhead.h"//test

using std::string;
using std::vector;
class STATION;
enum DIRECTION{from_start,from_end};//列车行驶方向

struct SAME_POS//辅助结构，用来描述两支线路存在的同名站点的情况
{
    int called_pos;//调用对象中的同名站点
    int other_pos;//参数对象中的同名站点位置
};

class LINE//地铁线路
{
    friend class SUBWAY;
private:
    string name;//线路名
    STATION*er_start;//遍历用起点站，只有一个站点时起点站与终点站指向同一对象，没有站点时两指针为空
    STATION*er_end;//遍历用终点站
    vector<STATION*>p_stations;//包含站点
    DIRECTION direction;
    int count;//站点数
    int GetPos(const STATION*p_st)const;//辅助方法，获取p_st在调用对象中的位置,p_st不属于调用对象时返回-1,当调用对象为空且参数也为NULL时返回0  complete
    int GetPos(const string&st_name)const;//功能同上，重载  complete
    bool GoToNextSt(const STATION*&p_st)const;//辅助方法，从p_st指向的站点开始单步遍历站点，并将遍历到的站点指针值赋给p_st若单步移动失败，则返回false，同时p_st设为0  complete
    bool GoToPrevSt(const STATION*&p_st)const;//辅助方法，从p_st指向的站点开始反向单步遍历站点，并将遍历到的站点指针值赋给p_st若单步移动失败，则返回false，同时p_st设为0  complete
    void ThrowIfSameName(STATION*p_st)const;//辅助方法，当p_st所指向的站点或该站点所属的线路存在同名对象时抛出异常   complete
    bool GetPrevAndNext(int pos,const STATION*&p_prev,const STATION*&p_next)const;//辅助方法，获取插入位点的前站指针和后站指针，当两指针都非NULL时才返回true  complete
    const STATION* PosToStation(int pos)const;//辅助函数，服务于重载[],通过位置获取站点，调用对象为空或pos超出范围时抛出异常 complete
    LINE(const LINE&);//隐藏复制构造函数

public:
    LINE(const string&arg_name);//构造线路对象，同时必须设置线路名称  complete
    vector<string> GetPath()const;//获取线路所有站点名 complete  已测
    string Name()const;//获取线路名  complete
    bool IsEmpty()const;//判断线路是否无站点  complete
    STATION*Find(const string&st_name)const;//通过站点名找到并返回站点指针,找寻失败则返回nullptr  complete
    vector<SAME_POS> GetSameStationPos(const LINE&line)const;//返回调用对象与line之间的同名站点位置结构  complete
    bool InsertStation(STATION*p_st,int pos);//插入站点，目前永远返回true,但出现同名现象或发现站点丢失时可抛出异常，只能插入没有所属线路的站点 complete  已测
    bool PushFstStation(STATION*p_st);//插入遍历用起点站,p_station必须指向动态内存,目前永远返回true,但出现同名现象时会抛出异常，只能插入没有所属线路的站点  complete  已测
    bool PushLstStation(STATION*p_st);//插入遍历用终点站,p_station必须指向动态内存,目前永远返回true,但出现同名现象时会抛出异常，只能插入没有所属线路的站点  complete  已测
    bool DeleteStation(int pos);//删除pos位置的站点，删除失败时返回false  complete  已测
    bool DeleteStation(const string&st_name);//功能同上，重载  complete
    bool PopStart();//删除并更新遍历用起点站，删除失败时返回false  complete  已测
    bool PopEnd();//删除并更新遍历用终点站，删除失败时返回false  已测
    void Clear();//清空线路  complete
    const STATION* operator[](int pos)const;//重载[],const版本,调用对象为空或pos超出范围时抛出异常 complete  已测
    STATION* operator[](int pos);//重载[],非const版本,调用对象为空或pos超出范围时抛出异常  complete  已测
    LINE& operator<<(const string&st_name);//重载<<，构造新站点并将其添加为终点站，内部调用PushLstStation complete  已测
    int Counter()const;//获取站点数量  complete  已测
    ~LINE();//视情况释放站点内存
};

#endif // LINE_H
