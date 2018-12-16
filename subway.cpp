#include "subway.h"
#include "station.h"
#include "line.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>
#include "sw_database.h"
//#include <ulimit.h>
using std::ofstream;
using std::ifstream;
using std::getline;
using std::istringstream;
using std::map;
using std::pair;
#define USE_SQL
//SUBWAY methods
void SUBWAY::ThrowIfSameName(const LINE &line) const//辅助方法，当line与in_use_lines或un_use_lines里的元素有重名时抛出异常，异常指出是在哪个vector<LINE>对象中出现的重名
{
    for(const LINE*in_line:in_use_lines)
    {
        if(in_line->Name()==line.Name())
            throw string("运行线路里已有同名线路");
    }
    for(const LINE*un_line:un_use_lines)
    {
        if(un_line->Name()==line.Name())
            throw string("备用线路里已有同名线路");
    }
}

int SUBWAY::GetPos(const std::string &line_name, MODE mode) const//辅助方法，通过线路名获取相应mode的线路簇位置,位置获取失败时返回-1
{
    vector<LINE*>lines;
    AssignLines(lines,mode);

    for(unsigned i=0;i<lines.size();i++)
    {
        if(lines[i]->Name()==line_name)
            return i;
    }
    return -1;
}

void SUBWAY::AssignLines(vector<LINE *> &lines, MODE mode) const//辅助方法，依据mode将lines赋值为in_use_lines或un_use_lines
{
    if(mode==UN_USE)
        lines=un_use_lines;
    else if(mode==IN_USE)
        lines=in_use_lines;
}

vector<PATH> SUBWAY::GetAllPaths(const STATION *p_begin, const STATION *p_end,vector<const STATION*>&p_igns)const//辅助方法，获取两站点之间的所有线路，当下一次递归调用的第一个参数的值为p_igns中的元素时，忽略掉该次递归调用，任意p_begin或p_end为nullptr时抛出异常
{
  //函数思想：该函数每次被调用都将p_begin的值存储在数组p_igns里用以忽略掉一部分递归调用，即当p_begin的下一站p_next出现在p_igns中时
  //        略过将p_next作为p_begin的递归调用，而该函数每次有返回时则删除最后一个元素，以免其他路径忽略掉不该忽略的递归调用

    if(!p_begin||!p_end)
        throw string("站点不存在");

    p_igns.push_back(p_begin);

    if(p_begin==p_end)
        return vector<PATH>{PATH{p_begin->name}};//若起点站即目的地站，则直接返回该站点名构成的返回对象

    vector<PATH>sub_paths;
    for(const STATION*p_next:p_begin->p_next_sts)//循环递归，获取所有下一站到目的地站的所有线路
    {
        if(std::count(p_igns.begin(),p_igns.end(),p_next))
            continue;
        MergePathVec(sub_paths,GetAllPaths(p_next,p_end,p_igns));
        p_igns.pop_back();//函数每次返回后都去掉最后一个忽略站点标记，以免其他路径的递归在到达相同站点时直接略过递归调用
    }
    for(PATH&path:sub_paths)//将本起点站名从头添加进所有下一站到目的地站的线路中，即构成本起点站到目的地站的所有线路
        path.insert(path.begin(),p_begin->name);

    return sub_paths;
}

PATH_MSG SUBWAY::GetPathMsg(const PATH &path, CP_TYPE ct) const//辅助方法，通过路径获取更多信息，并构造出PATH_MSG对象
{
    if(path.size()==1)
        return PATH_MSG(path,0,0,vector<string>(0),vector<string>(0),ct);

    vector<vector<string>> uf_order=UF_GetLinesOrder(path);//获取未整理的线路次序

    vector<string> lines_order=GetNullAmbFromUf(uf_order);//获取未去重的没有二义性的线路次序

    vector<string>tr_point;//存储换乘站点
    for(unsigned int i=1;i<lines_order.size();i++)
    {
        if(lines_order[i]!=lines_order[i-1])
            tr_point.push_back(path[i]);
    }
    vector<string>::iterator it=std::unique(lines_order.begin(),lines_order.end());//去重
    lines_order.erase(it,lines_order.end());//去重后去尾获取线路次序
    int tr_count=lines_order.size()-1;//换乘次数
    int ps_count=path.size()-1;//经站数量

    return PATH_MSG(path,tr_count,ps_count,tr_point,lines_order,ct);

}

vector<vector<std::string> > SUBWAY::UF_GetLinesOrder(const PATH &path) const//辅助方法，获取指定路径的未整理的线路次序，服务于GetLinesOrder方法
{
    vector<vector<string>>lines_order;
    for(unsigned int i=0;i<path.size()-1;i++)
    {
        STATION*p_cur=GetStation(path[i],IN_USE);
        STATION*p_next=GetStation(path[i+1],IN_USE);
        vector<string>shared_lines=p_cur->NameOfSharedLines(p_next);
        vector<string>amb_lines;
        for(vector<string>::iterator it=shared_lines.begin();it!=shared_lines.end();it++)
        {
            const LINE&line=GetLine(*it,IN_USE);
            int pos_cur=line.GetPos(p_cur);
            int pos_next=line.GetPos(p_next);
            int diff_pos=pos_cur-pos_next;
            if(diff_pos==1||diff_pos==-1)
                amb_lines.push_back(*it);
        }
        lines_order.push_back(amb_lines);
    }
    return lines_order;
}

vector<std::string> SUBWAY::GetNullAmbFromUf(const vector<vector<std::string> > uf_order) const//辅助方法，通过未整理的线路次序获取未去重的，没有二义性的线路次序
{
    vector<vector<string>>orders=GetAllOders(uf_order);//获取没有二义性的所有未去重的线路次序
    vector<string>lines_order;//用以存储最少换乘次数的未去重线路次序
    int min_tr_c=uf_order.size()+1;//min_tr_c将获取到最少换乘次数，但现在先初始化为比所有可能的换乘次数都大的值
    for(const vector<string>&od:orders)
    {
        int tr_c=0;//换乘次数
        for(unsigned int i=1;i<od.size();i++)
        {
            if(od[i]!=od[i-1])
                tr_c++;
        }
        if(tr_c<min_tr_c)
        {
            lines_order=od;
            min_tr_c=tr_c;
        }
    }
    return lines_order;
}

vector<vector<std::string> > SUBWAY::GetAllOders(const vector<vector<std::string> > &amb_order) const//辅助方法，将由UF_GetLinesOrder获得的有二义性的线路次序整理成多个没有二义性的线路次序
{
    vector<vector<string>>oders;
    if(!amb_order.size())
        return oders;
    if(amb_order.size()==1)
    {
        for(const string&line:amb_order[0])
            oders.push_back(vector<string>(1,line));
        return oders;
    }
    vector<vector<string>>sub_amb_order=amb_order;
    sub_amb_order.pop_back();
    vector<vector<string>>sub_oders=GetAllOders(sub_amb_order);
    for(const string&line:amb_order.back())
    {
        for(vector<string>od:sub_oders)
        {
            od.push_back(line);
            oders.push_back(od);
        }
    }
    return oders;
}

void SUBWAY::MergePathVec(vector<PATH> &main_paths, const vector<PATH> &part_paths) const//辅助方法，拼接两参数对象
{
    for(const PATH&path:part_paths)
        main_paths.push_back(path);
}

void SUBWAY::Earliest(PATH_MSG &pm) const//最早化转乘站点
{
    vector<PATH_MSG::SELECTING_TRPS>sls=pm.GetSelectingTrps();//获取所有转乘站点与邻站（靠近起点站）所构成的path子集，以及转乘所至的线路
    for(unsigned int i=0;i<sls.size();i++)
    {
        while(IsContainedSet(sls[i].sl_tr_points,sls[i].tr_line))//若该子集也同时为转乘所至线路的子集
            pm.Earlier(sls[i].sl_tr_points);//则将该子集沿着path向左移动一位，以更新子集的元素
        pm.tr_points[i]=(sls[i].sl_tr_points)[1];//若该子集不为转乘所至线路的子集，则将对应的转乘站点设置为该子集中的第二个元素
    }
}

bool SUBWAY::IsContainedSet(vector<std::string> sl_tr_points, const std::string &tr_line) const//判断第一个参数或者它的反转对象是否为第二个参数的子集，由于该函数可能会反转sl_tr_points，因此按值传参
{
    const PATH path=GetLine(tr_line,IN_USE).GetPath();
    bool is_subset=std::search(path.begin(),path.end(),sl_tr_points.begin(),sl_tr_points.end())!=path.end();
    if(is_subset)
        return true;
    else
    {
        std::reverse(sl_tr_points.begin(),sl_tr_points.end());
        is_subset=std::search(path.begin(),path.end(),sl_tr_points.begin(),sl_tr_points.end())!=path.end();
        return is_subset;
    }
}

void SUBWAY::WriteFile() const//将轨道信息写入文件
{
    ofstream ofs(filename);
    ofs<<"#IN_USE"<<endl;
    WriteLines(ofs,in_use_lines);
    ofs<<"#UN_USE"<<endl;
    WriteLines(ofs,un_use_lines);
    ofs.close();
}

void SUBWAY::WriteSql() const//将线路写入数据库
{
    WriteSql(in_use_lines,"InuseLines");
    WriteSql(un_use_lines,"UnuseLines");
    sw_database->UseDatabase("InuseLines");
    sw_database->Cover();//备份表格转正
    sw_database->UseDatabase("UnuseLines");
    sw_database->Cover();//备份表格转正
}

void SUBWAY::WriteSql(vector<LINE *> lines, const std::string &db_name)const//将线路写入特定数据库
{
    sw_database->UseDatabase(db_name);
    for(unsigned int i=0;i<lines.size();i++)
    {
        string tab_name=lines[i]->Name()+string("_bk");//生成备份表格名
        sw_database->CreateTable(tab_name);//创建备份表格
        PATH path=lines[i]->GetPath();
        for(unsigned int s=0;s<path.size();s++)
            sw_database->PushItem(tab_name,path[s]);
    }
}

void SUBWAY::WriteLines(ofstream &ofs, const vector<LINE *>&lines) const//将多条线路写入文件
{
    for(const LINE*p_line:lines)
    {
        ofs<<"LineName:"<<p_line->Name()<<endl;
        ofs<<"StationName:";
        PATH path=p_line->GetPath();
        for(const string&st_name:path)
            ofs<<st_name<<'-';
        ofs<<'\n';
    }
}

void SUBWAY::ReadFile()//从文件里读出轨道信息
{
    ifstream ifs(filename);
    ReadInuseToUnuse(ifs);
    while(un_use_lines.size())
        MoveLineToInUse(0);
    ReadUnuse(ifs);
    ifs.close();
}

void SUBWAY::ReadInuseToUnuse(ifstream &ifs)//将文件中的inuse线路读取到本对象的un_use_lines中去
{
    string temp;
    if(BeginWithIN_USE(ifs))
    {
        while(BeginWithLineName(ifs))
        {
            getline(ifs,temp,'\n');
            CreatLineToUnUse(temp);
            if(BeginWithStationName(ifs))
                ReadStationsToUnuse(ifs);
            else
                break;
        }
    }
}

bool SUBWAY::BeginWithIN_USE(ifstream &ifs)//判断文件读取指针下次是否读取到"#IN_USE"
{
    string temp;
    getline(ifs,temp,'\n');
    return temp==string("#IN_USE");
}

bool SUBWAY::BeginWithLineName(ifstream &ifs)//判断文件读取指针下次是否读取到"LineName:"
{
    string temp;
    getline(ifs,temp,':');
    return temp==string("LineName");
}

bool SUBWAY::BeginWithStationName(ifstream &ifs)//判断文件读取指针下次是否读取到"StationName"
{
    string temp;
    getline(ifs,temp,':');
    return temp==string("StationName");
}

void SUBWAY::ReadStationsToUnuse(ifstream &ifs)//读取站点到un_use_lines.back()中
{
    string all;
    getline(ifs,all,'\n');
    istringstream instr(all);
    string temp;
    getline(instr,temp,'-');
    while(!instr.eof())
    {
        *(un_use_lines.back())<<temp;
        getline(instr,temp,'-');
    }
}

void SUBWAY::ReadUnuse(ifstream &ifs)//读取文件中的unuse线路
{
    string temp;
    if(HaveUN_USE(ifs))
    {
        while(BeginWithLineName(ifs))
        {
            getline(ifs,temp,'\n');
            CreatLineToUnUse(temp);
            if(BeginWithStationName(ifs))
                ReadStationsToUnuse(ifs);
            else
                break;
        }
    }
}

bool SUBWAY::HaveUN_USE(ifstream &ifs)//判断能否从文件中读取到"#UN_USE"
{
    string temp;
    ifs.seekg(0,std::ios::beg);
    getline(ifs,temp,'\n');
    while(ifs)
    {
        if(temp==string("#UN_USE"))
            return true;
        getline(ifs,temp,'\n');
    }
    return false;
}

void SUBWAY::ReadSql()//从数据库里读出轨道信息
{
    ReadSqlToUnuse("InuseLines");//将数据库中的运行线路读取到未运行线路数组里
    while(un_use_lines.size())//转移未运行线路里的数组
        MoveLineToInUse(0);
    ReadSqlToUnuse("UnuseLines");
}

void SUBWAY::ReadSqlToUnuse(const std::string &db_name)//将数据库里的线路读取到未运行线路组中
{
    sw_database->UseDatabase(db_name);//选择运行线路数据库
    vector<string>tab_names=sw_database->GetTableNames();//获取所有表名（线路名）
    for(unsigned int i=0;i<tab_names.size();i++)
    {
        CreatLineToUnUse(tab_names[i]);//创建空线路
        int st_count=sw_database->GetItemCount(tab_names[i]);//获取表格的单元数量（站点数量）
        for(int j=0;j<st_count;j++)
            (*(un_use_lines.back()))<<sw_database->GetValue(tab_names[i],j);//创建新站点
    }
}

void SUBWAY::SortLines(vector<LINE *> &lines)//对线路进行排序
{
    map<string,LINE*>sort_lines;
    for(LINE*p_line:lines)
        sort_lines.insert(pair<string,LINE*>(p_line->Name(),p_line));
    vector<LINE*>temp_lines;
    for(pair<string,LINE*>p:sort_lines)
        temp_lines.push_back(p.second);
    lines=temp_lines;
}

void SUBWAY::SortOwnLines()//对所有拥有的线路按名称做字典进行排序
{
    SortLines(in_use_lines);
    SortLines(un_use_lines);
}

SUBWAY::SUBWAY(const string&fn):filename(fn),sw_database(new SW_DATABASE)
{
#ifdef USE_SQL
    ReadSql();
    qDebug()<<"read sql";
#else
    ReadFile();
    qDebug()<<"read file";
#endif
}

vector<PATH> SUBWAY::GetAllUseStatitonNames() const//获得所有运行线路的站点名，每个PATH对象的第一个元素为线路名，以此将站点分类为不同线路
{
    vector<PATH> all_ways;
    for(const LINE*line:in_use_lines)
    {
        all_ways.push_back(PATH(1,line->Name()));
        PATH path=line->GetPath();
        for(const string&st_name:path)
            all_ways.rbegin()->push_back(st_name);
    }
    return all_ways;
}

PATH SUBWAY::GetStationNamesFromLine(const string &line_name, MODE mode) const//获取指定使用或未使用线路的所有站点名
{
    vector<LINE*>lines;
    AssignLines(lines,mode);

    for(const LINE*line:lines)
    {
        if(line->Name()==line_name)
            return line->GetPath();
    }
    return PATH();
}

vector<std::string> SUBWAY::GetLineNamesFromStation(const string &station, MODE mode) const//获取指定站点所属的运行线路或未运行线路
{
    vector<string>names;
    vector<LINE*>lines;
    AssignLines(lines,mode);

    const STATION*p_st=nullptr;
    for(const LINE*line:lines)
    {
        p_st=line->Find(station);
        if(p_st)
            names.push_back(line->Name());
    }
    return names;
}

vector<std::string> SUBWAY::GetLineNamesFromStation(const std::string &station) const//功能同上，没有mode限制
{
    vector<string>names;
    for(const LINE*line:in_use_lines)
    {
        if(line->Find(station))
            names.push_back(line->Name());
    }
    for(const LINE*line:un_use_lines)
    {
        if(line->Find(station))
            names.push_back(line->Name());
    }
    return names;
}

vector<std::string> SUBWAY::GetAllLineNames(MODE mode)const//获取所有使用或未使用线路名称
{
    vector<LINE*>lines;
    AssignLines(lines,mode);

    PATH line_names;
    for(const LINE*line:lines)
        line_names.push_back(line->Name());
    return line_names;

}

vector<PATH_MSG> SUBWAY::GetTopPaths(const std::string &begin_nm, const std::string &end_nm, int top, CP_TYPE ct) const//依据ct,获取top个最少换乘线路或最少经站线路，参数站点名未找到时抛出异常
{
    STATION*p_begin=GetStation(begin_nm,IN_USE);
    STATION*p_end=GetStation(end_nm,IN_USE);
    vector<const STATION*>sv(0);
    vector<PATH>all_paths=GetAllPaths(p_begin,p_end,sv);//获取到达目的地的所有线路的经站名

    if(all_paths.size()==0)
        return vector<PATH_MSG>(0);
    multiset<PATH_MSG>path_msg_set;
    for(PATH path:all_paths)//将描述所有线路的数组对象复制转换为multiset<PATH_MSG>类对象
        path_msg_set.insert(GetPathMsg(path,ct));

    vector<PATH_MSG>ret;//用于存储前top位的线路信息(PATH_MSG)对象
    int i=0;
    for(auto it=path_msg_set.begin();it!=path_msg_set.end();it++)
    {
        if(i++>=top)
            break;
        Earliest(const_cast<PATH_MSG&>(*it));//最早话换乘站点
        ret.push_back(*it);
    }
    return ret;
}

STATION *SUBWAY::GetStation(const std::string &st_name, MODE mode) const//通过站点名获取该站点指针,获取失败时返回nullptr
{
    vector<LINE*>lines;
    AssignLines(lines,mode);

    for(LINE*p_line:lines)
    {
        STATION*p_st=p_line->Find(st_name);
        if(p_st)
            return p_st;
    }
    return nullptr;
}

STATION *SUBWAY::GetStation(const std::string &st_name) const//功能同上，重载版本，不需MODE
{
    STATION*p_st;
    p_st=GetStation(st_name,IN_USE);
    if(!p_st)
        p_st=GetStation(st_name,UN_USE);
    return p_st;
}

STATION *SUBWAY::GetStation(const std::string &line_name, const std::string &st_name)//获取线路line_name中的st_name站点的指针,线路没找到时抛出异常，站点没找到时返回nullptr
{
    return GetLine(line_name).Find(st_name);
}

FIND SUBWAY::Search(const std::string &obj_name) const//获取搜索到的元素信息对象
{
    string type;
    int count=0;
    MODE mode=GetModeNullThrow(obj_name);
    if(mode!=ERR)
    {
        type="LINE";
        count=1;
    }
    else
    {
        for(const LINE*p_line:in_use_lines)
        {
            if(p_line->Find(obj_name))
                count++;
        }
        for(const LINE*p_line:un_use_lines)
        {
            if(p_line->Find(obj_name))
                count++;
        }
        if(count)
            type="STATION";
    }
    return FIND(type,count);
}

int SUBWAY::GetStPos(const std::string &st_name, const std::string &line_name)//获取名为st_name的站点在名为line_name的线路中的位置,未找到线路时抛出异常,未找到站点时返回-1
{
    return GetLine(line_name).GetPos(st_name);
}

bool SUBWAY::CreatLineToUnUse(const string&line_name)//创建一条空的LINE到un_use_lines族里，若线路名重复则抛出异常，目前永远返回true
{
    if(line_name.empty())
        throw string("请输入线路名称");
    LINE*p_line=new LINE(line_name);
    ThrowIfSameName(*p_line);
    un_use_lines.push_back(p_line);
    return true;
}

bool SUBWAY::MoveLineToInUse(int un_use_index)//将下标为un_use_index的un_use_lines的元素投入使用,可抛出异常，目前永远返回true
{
    if(un_use_index<0||unsigned(un_use_index)>=un_use_lines.size())
        throw string("参数下标超出范围");

    LINE*move_line=un_use_lines[un_use_index];//获取待使用线路指针
    un_use_lines.erase(un_use_lines.begin()+un_use_index);//删除未使用线路数组中的待使用线路指针

    for(LINE*use_line:in_use_lines)//依次遍历运行线路
    {
        vector<SAME_POS>same_poses=use_line->GetSameStationPos(*move_line);//获取运行线路与待使用线路的同名站点位置
        for(const SAME_POS&same_pos:same_poses)//遍历所有同名站点结构
        {
            move_line->DeleteStation(same_pos.other_pos);//待使用线路删除自己的同名站点
            move_line->InsertStation((*use_line)[same_pos.called_pos],same_pos.other_pos);//将运行线路的同名站点插入待使用线路
        }
    }
    in_use_lines.push_back(move_line);//将待使用线路添加进运行线路数组
    SortOwnLines();//对线路重新排序
    return true;
}

bool SUBWAY::MoveLineToInUse(const std::string &line_name)//功能同上，但是依据线路名移动线路，内部调用此函数的下标参数版本，未找到线路时抛出异常，目前永远返回true
{
    int pos=GetPos(line_name,UN_USE);
    if(pos==-1)
        throw string("线路不存在");
    return MoveLineToInUse(pos);
}

bool SUBWAY::MoveLineToUnUse(const std::string &line_name)//暂停运行线路的使用，可抛出异常，目前永远返回true
{
    LINE&line=GetLine(line_name,IN_USE);//获取线路对象的引用，可抛出异常
    for(STATION*p_st:line.p_stations)//遍历该线路的所有站点
    {
        if(p_st->blt_line.size()>=2)//若站点至少有2个所属线路
        {
            int pos=line.GetPos(p_st);//获取该站点在线路中的位置
            line.DeleteStation(pos);//删除该位置的站点

            STATION*p_co=new STATION(p_st->name);//新建同名站点
            line.InsertStation(p_co,pos);//在同样的位置插入新建的同名站点
        }
    }
    int pos_line=GetPos(line_name,IN_USE);//获取线路的位置
    in_use_lines.erase(in_use_lines.begin()+pos_line);
    un_use_lines.push_back(&line);
    SortOwnLines();//对线路重新排序
    return true;
}

bool SUBWAY::AddStation(const std::string &st_name, const std::string &line_name, int pos)//增加名为st_name的站点到名为line_name线路的pos位置上，其内部调用函数可抛出异常
{
    MODE mode=GetMode(line_name);
    LINE&line=GetLine(line_name);
    if(pos==-1)
        pos=line.Counter();
    if(mode==IN_USE)
    {
        MoveLineToUnUse(line_name);
        line.InsertStation((new STATION(st_name)),pos);
        MoveLineToInUse(line_name);
    }
    else if(mode==UN_USE)
        line.InsertStation((new STATION(st_name)),pos);
    else
        return false;
    return true;
}

LINE *SUBWAY::GetLine(int line_index, MODE mode)//获取in_use_lines或un_use_lines中的一条线路指针,line_index超出范围则抛出异常，同时返回nullptr
{
    if(mode==UN_USE)
    {
        if(line_index<0||line_index>=int(un_use_lines.size()))
            throw string("获取运行线路的下标超出范围");
        return un_use_lines[line_index];
    }
    else if(mode==IN_USE)
    {
        if(line_index<0||line_index>=int(in_use_lines.size()))
            throw string("获取备用线路的下标超出范围");
        return in_use_lines[line_index];
    }
    throw string("无法确定线路簇");
    return nullptr;
}

LINE &SUBWAY::GetLine(const std::string &line_name, MODE mode)//功能同上，但通过线路名返回引用，查找失败时抛出异常
{
    int pos=GetPos(line_name,mode);
    if(pos==-1)
        throw string("该线路不存在");
    return *GetLine(pos,mode);
}

const LINE &SUBWAY::GetLine(const std::string &line_name, MODE mode)const
{
    return const_cast<SUBWAY*>(this)->GetLine(line_name,mode);
}

LINE &SUBWAY::GetLine(const std::string &line_name)
{
    for(LINE*p_line:in_use_lines)
    {
        if(p_line->Name()==line_name)
            return *p_line;
    }
    for(LINE*p_line:un_use_lines)
    {
        if(p_line->Name()==line_name)
            return *p_line;
    }
    throw string("线路不存在");
}

bool SUBWAY::DeleteLine(const std::string &line_name, MODE mode)//删除线路,可抛出异常
{
    int pos=GetPos(line_name,mode);
    if(pos==-1)
        throw string("该线路不存在");
    LINE*p_line=GetLine(pos,mode);
    if(mode==IN_USE)
        in_use_lines.erase(in_use_lines.begin()+pos);
    else if(mode==UN_USE)
        un_use_lines.erase(un_use_lines.begin()+pos);
    delete p_line;
    return true;
}

bool SUBWAY::DeleteStation(const std::string &st_name, const std::string &line_name)//删除线路line_name中的站点st_name
{
    LINE&line=GetLine(line_name);
    int pos=line.GetPos(st_name);
    if(pos==-1)
        throw string("站点不存在");
    line.DeleteStation(pos);
    return true;
}

MODE SUBWAY::GetMode(const std::string &line_name) const//获取线路的运行状态,线路未找到时抛出异常
{
    if(GetPos(line_name,IN_USE)!=-1)
        return IN_USE;
    else if(GetPos(line_name,UN_USE)!=-1)
        return UN_USE;
    else
        throw string("线路不存在");
}

MODE SUBWAY::GetModeNullThrow(const std::string &line_name)const
{
    if(GetPos(line_name,IN_USE)!=-1)
        return IN_USE;
    else if(GetPos(line_name,UN_USE)!=-1)
        return UN_USE;
    else
        return ERR;
}

SUBWAY::~SUBWAY()
{
#ifdef USE_SQL
    WriteSql();
    qDebug()<<"write sql";
#elif defined FILE_TO_SQL
    sw_database->SetFileToSql(true);
    WriteSql();
    qDebug()<<"write sql";
#else
    WriteFile();
    qDebug()<<"write file";
#endif
    for(LINE*p_line:in_use_lines)
        delete p_line;
    for(LINE*p_line:un_use_lines)
        delete p_line;

   delete sw_database;
}


//PATH_MSG methods
vector<PATH_MSG::SELECTING_TRPS> PATH_MSG::GetSelectingTrps() const//辅助方法，获取所有转乘站点与邻站（靠近起点站）所构成的path子集，以及转乘所至的线路
{
    vector<SELECTING_TRPS>sls;
    for(unsigned int i=0;i<tr_points.size();i++)
    {
        vector<string>sl_tr_points{GetPrev(tr_points[i]),tr_points[i]};
        sls.push_back(SELECTING_TRPS{sl_tr_points,lines_order[i+1]});
    }
    return sls;
}

std::string PATH_MSG::GetPrev(const std::string &station) const//辅助方法，获取path中station的前一站,该私有方法不做异常处理，因此必须被封装调用以规避错误传参
{
    for(unsigned int i=1;i<path.size();i++)
    {
        if(path[i]==station)
            return path[i-1];
    }
    qDebug()<<"PATH_MSG::GoToPrev：参数错误";
    return "";
}

void PATH_MSG::Earlier(vector<std::string> &sl_tr_points) const//辅助方法，参数为path的子集，该方法使子集沿着path向左移动一位，以更新该子集
{
    for(string&station:sl_tr_points)
        station=GetPrev(station);
}

PATH_MSG::PATH_MSG(const PATH &other_path, int tr_c, int ps_c, const vector<std::string> &tr_p, const vector<std::string> &li_od, CP_TYPE ct)
    :path(other_path),tr_count(tr_c),ps_count(ps_c),tr_points(tr_p),lines_order(li_od),compare_type(ct){}

bool PATH_MSG::operator<(const PATH_MSG &pm)const//依据compare_type做出相应的比较,若两比较对象的compare_type不同，则抛出异常
{
    if(compare_type!=pm.compare_type)
        throw string("PATH_MSG：比较类型不一致");
    if(compare_type==TRANSFER)
        return vector<int>{tr_count,ps_count}<vector<int>{pm.tr_count,pm.ps_count};//字典比较
    else if(compare_type==PASS)
        return vector<int>{ps_count,tr_count}<vector<int>{pm.ps_count,pm.tr_count};
    throw string("PATH_MSG：比较类型错误");
}

PATH PATH_MSG::GetPath() const
{
    return path;
}

int PATH_MSG::GetTransferTimes() const
{
    return tr_count;
}

int PATH_MSG::GetPassCount() const
{
    return ps_count;
}

vector<std::string> PATH_MSG::GetTransferPoint() const
{
    return tr_points;
}

vector<std::string> PATH_MSG::GetLinesOrder() const
{
    return lines_order;
}

int PATH_MSG::GetPos(const std::string &st) const//获取站点在路径中的下标,没找到时返回-1
{
    for(unsigned int i=0;i<path.size();i++)
    {
        if(path[i]==st)
            return i;
    }
    return -1;
}
