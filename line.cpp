//线路类
//处理对站点的各项操作
#include "line.h"
#include "station.h"
int LINE::GetPos(const STATION *p_st)const//辅助方法，获取p_st在调用对象中的位置，p_st不属于调用对象时返回-1，当调用对象为空且参数也为NULL时返回0
{
    int i=0;
    for(STATION*p:p_stations)
    {
        if(p==p_st)
            return i;
        i++;
    }
    return -1;
}

int LINE::GetPos(const std::string &st_name) const//功能同上，重载
{
    for(unsigned int i=0;i<p_stations.size();i++)
    {
        if(p_stations[i]->name==st_name)
            return i;
    }
    return -1;
}

bool LINE::GoToNextSt(const STATION *&p_st) const//辅助方法，单步遍历站点，并将遍历到的站点指针值赋给p_st若单步移动失败，则返回false，同时p_st设为0
{
    int pos=GetPos(p_st);
    if(pos==-1||IsEmpty()||pos==count-1)
    {
        p_st=nullptr;
        return false;
    }
    p_st=p_stations[++pos];
    return true;
}

bool LINE::GoToPrevSt(const STATION *&p_st) const//辅助方法，从p_st指向的站点开始反向单步遍历站点，并将遍历到的站点指针值赋给p_st若单步移动失败，则返回false，同时p_st设为0
{
    int pos=GetPos(p_st);
    if(pos<=0||IsEmpty())
    {
        p_st=nullptr;
        return false;
    }
    p_st=p_stations[--pos];
    return true;
}

void LINE::ThrowIfSameName(STATION *p_st) const//辅助方法，当p_st所指向的站点或该站点所属的线路存在同名对象时抛出异常
{

    if(p_st->IsBelongedTo(Name()))
        throw string("站点已属于同名线路，拒绝添加");
    if(Find(p_st->name))
        throw string("线路中已有同名站点，拒绝添加");
}

bool LINE::GetPrevAndNext(int pos, const STATION *&p_prev, const STATION *&p_next) const//辅助方法，获取插入位点的前站指针和后站指针，当pos超出范围时返回false
{
    if(pos<0||pos>=count)
        return false;
    int pos_prev=pos-1;
    int pos_next=pos;

    if(pos_prev<0)
        p_prev=nullptr;
    else
        p_prev=p_stations[pos_prev];

    if(pos_next>=count)
        p_next=nullptr;
    else
        p_next=p_stations[pos_next];
    return true;
}

const STATION *LINE::PosToStation(int pos) const//辅助函数，服务于重载[],通过位置获取站点，调用对象为空或pos超出范围时抛出异常
{
    if(pos<0||pos>=count)
        throw string("参数超出范围");
    return p_stations[pos];
}

LINE::LINE(const string &arg_name):name(arg_name),er_start(nullptr),er_end(nullptr),direction(from_start),count(0){}

vector<string> LINE::GetPath() const//获取线路所有站点名
{
    vector<string>line_names;
    for(STATION*p_st:p_stations)
        line_names.push_back(p_st->name);

    return line_names;
}

string LINE::Name() const//获取线路名
{
    return name;
}

bool LINE::IsEmpty() const//判断线路是否无站点
{
    return !count;
}

STATION *LINE::Find(const string &st_name) const//通过站点名找到并返回站点指针，找寻失败则返回nullptr
{
    if(IsEmpty())
        return nullptr;
    for(STATION*p_st:p_stations)
    {
        if(p_st->name==st_name)
            return p_st;
    }
    return nullptr;
}

vector<SAME_POS> LINE::GetSameStationPos(const LINE &line) const//返回调用对象与line之间的同名站点位置结构
{
    vector<SAME_POS>same_poses;
    vector<string>other_st_names=line.GetPath();
    for(unsigned int called_i=0;called_i<p_stations.size();called_i++)
    {
        for(unsigned int other_i=0;other_i<other_st_names.size();other_i++)
        {
            if(p_stations[called_i]->name==other_st_names[other_i])
                same_poses.push_back(SAME_POS{called_i,other_i});
        }
    }
    return same_poses;
}

bool LINE::InsertStation(STATION *p_st, int pos)//插入站点，目前永远返回true,但出现同名现象或发现站点丢失时可抛出异常，只能插入没有所属线路的站点
{
    ThrowIfSameName(p_st);//出现同名现象则抛出异常

    if(pos>=count)
        return PushLstStation(p_st);//插入尾站点
    if(pos<=0)
        return PushFstStation(p_st);//插入头站点
    //假如非两端插入
    const STATION*p_next,*p_prev;
    GetPrevAndNext(pos,p_prev,p_next);//获取待插入站点的邻站

    const_cast<STATION*>(p_next)->Cut(const_cast<STATION*>(p_prev));//剪断邻近站点
    const_cast<STATION*>(p_prev)->Connect(p_st);//连接
    p_st->Connect(const_cast<STATION*>(p_next));//连接

    p_stations.insert(p_stations.begin()+pos,p_st);

    p_st->blt_line.push_back(Name());//更新插入站点所属线路名
    count++;

    return true;
}

bool LINE::PushFstStation(STATION *p_st)//插入遍历用起点站,p_station必须指向动态内存,目前永远返回true,但出现同名现象时会抛出异常，只能插入没有所属线路的站点
{
    ThrowIfSameName(p_st);//出现同名现象则抛出异常
    if(IsEmpty())//线路为空的情况
        er_start=er_end=p_st;
    else
    {
        p_st->Connect(er_start);//连接
        er_start=p_st;//头站点指针指向新插入站点
    }

    p_stations.insert(p_stations.begin(),p_st);
    p_st->blt_line.push_back(Name());//更新插入站点所属线路名
    count++;
    return true;
}

bool LINE::PushLstStation(STATION *p_st)//插入遍历用终点站,p_station必须指向动态内存,目前永远返回true,但出现同名现象时会抛出异常，只能插入没有所属线路的站点
{
    ThrowIfSameName(p_st);//出现同名现象则抛出异常

    if(IsEmpty())//线路为空的情况
        er_start=er_end=p_st;
    else
    {
        er_end->Connect(p_st);//连接
        er_end=p_st;//尾站点指针指向新插入站点
    }

    p_stations.push_back(p_st);
    p_st->blt_line.push_back(Name());//更新插入站点所属线路名
    count++;
    return true;
}

bool LINE::DeleteStation(int pos)//删除pos位置的站点，删除失败时返回false
{
    if(pos<0||pos>=count)
        return false;

    if(!pos)
        return PopStart();
    if(pos==count-1)
        return PopEnd();

    STATION*p_st=operator [](pos);//获取指向删除站点的指针
    const STATION*p_prev,*p_next;
    p_prev=p_next=p_st;                //获取待删除站点指针的前后站指针
    GoToPrevSt(p_prev);
    GoToNextSt(p_next);

    int num_of_bl=p_prev->NumOfSharedLines(p_st);//获取前站与待删除站点的共同线路数量
    if(num_of_bl==1)//若它们只有一条共同线路
        const_cast<STATION*>(p_prev)->Cut(p_st);//剪断前站与待删除站
    num_of_bl=p_st->NumOfSharedLines(p_next);//获取后站与待删除站点的共同线路数量
    if(num_of_bl==1)//若它们只有一条共同线路
        p_st->Cut(const_cast<STATION*>(p_next));//剪断待删除站与后站
    const_cast<STATION*>(p_prev)->Connect(const_cast<STATION*>(p_next));//连接前后站

    p_stations.erase(p_stations.begin()+pos);
    p_st->DeleteBelongedLine(Name());//清除待删除站点的所属线路关系
    if(!p_st->blt_line.size())//若带删除站点被完全游离，则释放其占用的内存
        delete p_st;
    count--;//站点数减一
    return true;
}

bool LINE::DeleteStation(const std::string &st_name)//功能同上，重载
{
    int pos=GetPos(st_name);
    return DeleteStation(pos);
}

bool LINE::PopStart()//删除并更新遍历用起点站，删除失败时返回false
{
    if(!count)//若线路没有站点则返回false
        return false;
    const STATION*p_next=er_start;
    GoToNextSt(p_next);
    if(!p_next)//若该线路只有一个站点
    {
        er_start->DeleteBelongedLine(Name());
        count--;
        if(!er_start->blt_line.size())//若待删除站点被删除后被完全游离
            delete er_start;//则释放待删除站点
        p_stations.erase(p_stations.begin());
        er_start=er_end=nullptr;
        return true;
    }
    int num_of_bl=er_start->NumOfSharedLines(p_next);//获取后站与待删除站点的共同线路数量
    if(num_of_bl==1)
        er_start->Cut(const_cast<STATION*>(p_next));//若它们只有一条共同线路，则剪断它们
    er_start->DeleteBelongedLine(Name());
    count--;
    if(!er_start->blt_line.size())//若待删除站点被删除后被完全游离
        delete er_start;//则释放待删除站点
    p_stations.erase(p_stations.begin());
    er_start=const_cast<STATION*>(p_next);//更新起点站
    return true;
}

bool LINE::PopEnd()//删除并更新遍历用终点站，删除失败时返回false
{
    if(!count)//若线路没有站点则返回false
        return false;

    const STATION*p_prev=er_end;
    GoToPrevSt(p_prev);
    if(!p_prev)//若该线路只有一个站点
    {
        er_end->DeleteBelongedLine(Name());
        count--;
        if(!er_end->blt_line.size())//若待删除站点被删除后被完全游离
            delete er_end;//则释放待删除站点
        p_stations.pop_back();
        er_start=er_end=nullptr;
        return true;
    }

    int num_of_bl=p_prev->NumOfSharedLines(er_end);//获取前站与待删除站点的共同线路数量
    if(num_of_bl==1)
        const_cast<STATION*>(p_prev)->Cut(er_end);//若它们只有一条共同线路，则剪断它们
    er_end->DeleteBelongedLine(Name());
    count--;
    if(!er_end->blt_line.size())//若将删除站点被删除后被完全游离，则释放将删除站点
        delete er_end;
    p_stations.pop_back();
    er_end=const_cast<STATION*>(p_prev);//更新起点站
    return true;
}

void LINE::Clear()//清空线路
{
   while(PopStart());
}

const STATION *LINE::operator[](int pos) const//重载[].const版本,调用对象为空或pos超出范围时抛出异常
{
    return PosToStation(pos);
}

STATION *LINE::operator[](int pos)//重载[],非const版本,调用对象为空或pos超出范围时抛出异常
{
    return const_cast<STATION*>(PosToStation(pos));
}

LINE &LINE::operator<<(const std::string &st_name)//重载<<，构造新站点并将其添加为终点站，内部调用PushLstStation
{
    STATION*p_st=new STATION(st_name);
    PushLstStation(p_st);
    return *this;
}

int LINE::Counter() const
{
    return count;
}

LINE::~LINE()
{
    while(PopStart());
}

