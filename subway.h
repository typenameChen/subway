#ifndef SUBWAY_H
#define SUBWAY_H
#include <vector>
#include <string>
#include "line.h"
#include <set>
#include "find.h"

using std::vector;
using std::string;
using std::multiset;
class ofstream;
using std::ofstream;
class ifstream;
using std::ifstream;
class SW_DATABASE;
enum MODE{UN_USE,IN_USE,ERR};//标记运行线路
enum CP_TYPE{TRANSFER,PASS};//标记线路的比较类型
typedef vector<string> PATH;//本文件代码中vector<string>类通常用来表示路径
class PATH_MSG//路径及其换乘次数与经站数量
{
    friend class SUBWAY;
private:
    struct SELECTING_TRPS//由于在同一条路径中，换乘站点并非不可变，因此此类型即用来存储当前换乘站点及其某个邻站，以帮助确定更早的换乘站点
    {
        vector<string>sl_tr_points;//待选择的换乘站点
        string tr_line;//换乘所至的线路
    };
    PATH path;//路径
    int tr_count;//换乘次数
    int ps_count;//经站数量
    vector<string>tr_points;//换乘站点
    vector<string>lines_order;//线路次序
    CP_TYPE compare_type;//比较类型
    vector<SELECTING_TRPS> GetSelectingTrps()const;//辅助方法，获取所有转乘站点与邻站（靠近起点站）所构成的path子集，以及转乘所至的线路  complete
    string GetPrev(const string&station)const;//辅助方法，获取path中station的前一站,该私有方法不做异常处理，因此必须被封装调用以规避错误传参  complete
    void Earlier(vector<string>&sl_tr_points)const;//辅助方法，参数为path的子集，该方法使子集沿着path向左移动一位，以更新该子集  complete
public:
    PATH_MSG():tr_count(0),ps_count(0),compare_type(TRANSFER){}
    PATH_MSG(const PATH&other_path,int tr_c,int ps_c,const vector<string>&tr_p,const vector<string>&li_od,CP_TYPE ct=TRANSFER);  //complete
    bool operator<(const PATH_MSG&pm)const;//依据compare_type做出相应的比较,若两比较对象的compare_type不同，则抛出异常  complete
    PATH GetPath()const;//complete
    int GetTransferTimes()const;//complete
    int GetPassCount()const;//complete
    vector<string> GetTransferPoint()const;//complete
    vector<string>GetLinesOrder()const;//complete
    int GetPos(const string&st)const;//获取站点在路径中的下标,没找到时返回-1  complete
};

class SUBWAY
{
private:
    vector<LINE*>in_use_lines;//存储所有正使用线路的指针
    vector<LINE*>un_use_lines;//存储所有未使用线路的指针
    const string filename;//存储轨道信息的文件名
    SW_DATABASE*sw_database;//轨道数据库指针
    void ThrowIfSameName(const LINE&line)const;//辅助方法，当line与in_use_lines或un_use_lines里的元素有重名时抛出异常，异常指出是在哪个vector<LINE>对象中出现的重名 complete
    int GetPos(const string&line_name,MODE mode)const;//辅助方法，通过线路名获取相应mode的线路簇位置,位置获取失败时返回-1  complete
    void AssignLines(vector<LINE*>&lines,MODE mode)const;//辅助方法，依据mode将lines赋值为in_use_lines或un_use_lines complete
    vector<PATH> GetAllPaths(const STATION*p_begin, const STATION*p_end, vector<const STATION *> &p_igns)const;//辅助方法，获取两站点之间的所有线路，当下一次递归调用的第一个参数的值为p_igns中的元素时，忽略掉该次递归调用，任意p_begin或p_end为nullptr时抛出异常  complete
    PATH_MSG GetPathMsg(const PATH&path,CP_TYPE ct)const;//辅助方法，通过路径获取更多信息，并构造出PATH_MSG对象  complete
    vector<vector<string>>UF_GetLinesOrder(const PATH&path)const;//辅助方法，获取指定路径的未整理的线路次序  complete
    vector<string> GetNullAmbFromUf(const vector<vector<string>>uf_order)const;//辅助方法，通过未整理的线路次序获取未去重的，没有二义性的线路次序  complete
    vector<vector<string>>GetAllOders(const vector<vector<string>>&amb_order)const;//辅助方法，将由UF_GetLinesOrder获得的有二义性的线路次序整理成多个没有二义性的线路次序 complete
    void MergePathVec(vector<PATH>&main_paths,const vector<PATH>&part_paths)const;//辅助方法，拼接两参数对象  complete
    void Earliest(PATH_MSG&pm)const;//最早化转乘站点  complete
    bool IsContainedSet(vector<string>sl_tr_points,const string&tr_line)const;//判断第一个参数或者它的反转对象是否为第二个参数的子集，由于该函数可能会反转sl_tr_points，因此按值传参  complete
    //file interaction
    void WriteLines(ofstream&ofs, const vector<LINE *> &lines)const;//将多条线路写入文件 complete
    void ReadFile();//从文件里读出轨道信息  complete
    void ReadInuseToUnuse(ifstream&ifs);//将文件中的inuse线路读取到本对象的un_use_lines中去  complete
    bool BeginWithIN_USE(ifstream&ifs);//判断文件读取指针下次是否读取到"#IN_USE" complete
    bool BeginWithLineName(ifstream&ifs);//判断文件读取指针下次是否读取到"LineName"  complete
    bool BeginWithStationName(ifstream&ifs);//判断文件读取指针下次是否读取到"StationName"  complete
    void ReadStationsToUnuse(ifstream&ifs);//读取站点到un_use_lines.back()中  complete
    void ReadUnuse(ifstream&ifs);//读取文件中的unuse线路  complete
    bool HaveUN_USE(ifstream&ifs);//判断能否从文件中读取到"#UN_USE"  complete
    //sql interaction 运行与未运行线路分别由不同数据库维护，每张表维护一条线路
    void ReadSql();//从数据库里读出轨道信息  complete
    void ReadSqlToUnuse(const string&db_name);//将数据库里的线路读取到未运行线路组中  complete

    void SortLines(vector<LINE*>&lines);//对线路进行排序  complete
    void SortOwnLines();//对所有拥有的线路按名称做字典进行排序  complete

public:
    SUBWAY(const std::string &fn="info/subway_info.txt");
    //file interaction
    void WriteFile()const;//将轨道信息写入文件  complete
    //sql interaction
    void WriteSql()const;//将线路写入数据库  complete 
    void WriteSql(vector<LINE*>lines,const string&db_name)const;//将线路写入特定数据库  complete
    //查询操作
    vector<PATH> GetAllUseStatitonNames()const;//获得所有运行线路的站点名，每个PATH对象的第一个元素为线路名，以此将站点分类为不同线路  complete
    PATH GetStationNamesFromLine(const string&line_name,MODE mode)const;//获取指定使用或未使用线路的所有站点名 complete
    vector<string> GetLineNamesFromStation(const string&station,MODE mode)const;//获取指定站点所属的运行线路或未运行线路 complete
    vector<string> GetLineNamesFromStation(const string&station)const;//功能同上，没有mode限制  complete
    vector<string> GetAllLineNames(MODE mode)const;//获取所有使用或未使用线路名称  complete
    vector<PATH_MSG> GetTopPaths(const string&begin_nm,const string&end_nm,int top,CP_TYPE ct) const;//依据ct,获取top个最少换乘线路或最少经站线路，参数站点名未找到时抛出异常  complete
    STATION* GetStation(const string&st_name,MODE mode)const;//通过站点名获取该站点指针,获取失败时返回nullptr  complete
    STATION* GetStation(const string&st_name)const;//只适用于UN_USE和IN_USE之中没有同名站点的情况，除此之外功能同上，重载版本，不需MODE  complete
    STATION* GetStation(const string&line_name,const string&st_name);//获取线路line_name中的st_name站点的指针,线路没找到时抛出异常，站点没找到时返回nullptr  complete
    FIND Search(const string&obj_name)const;//获取搜索到的元素信息对象 complete
    int GetStPos(const string&st_name,const string&line_name);//获取名为st_name的站点在名为line_name的线路中的位置,未找到线路时抛出异常,未找到站点时返回-1  complete
    //修改操作
    bool CreatLineToUnUse(const string&line_name);//创建一条空的LINE到un_use_lines族里，若线路名重复则抛出异常，目前永远返回true  complete
    bool MoveLineToInUse(int un_use_index);//将下标为un_use_index的un_use_lines的元素投入使用,可抛出异常，目前永远返回true  complete
    bool MoveLineToInUse(const string&line_name);//功能同上，但是依据线路名移动线路，内部调用此函数的下标参数版本，未找到线路时抛出异常，目前永远返回true
    bool MoveLineToUnUse(const string&line_name);//暂停运行线路的使用，可抛出异常，目前永远返回true  complete
    bool AddStation(const string&st_name,const string&line_name,int pos=-1);//增加名为st_name的站点到名为line_name线路的pos位置上，其内部调用函数可抛出异常  complete
    LINE* GetLine(int line_index, MODE mode);//获取in_use_lines或un_use_lines中的一条线路指针,line_index超出范围则抛出异常，同时返回nullptr  complete
    LINE& GetLine(const string&line_name,MODE mode);//功能同上，但通过线路名返回引用，查找失败时抛出异常  complete
    const LINE&GetLine(const string &line_name, MODE mode)const;//  complete
    LINE& GetLine(const string&line_name);//没有MODE限定，可抛出异常  complete
    bool DeleteLine(const string&line_name,MODE mode);//删除线路,可抛出异常 complete
    bool DeleteStation(const string&st_name,const string&line_name);//删除线路line_name中的站点st_name  complete
    MODE GetMode(const string&line_name)const;//获取线路的运行状态,线路未找到时抛出异常  complete
    MODE GetModeNullThrow(const string&line_name)const;

    ~SUBWAY();//释放所有内存
};

#endif // SUBWAY_H

//1 管理员可向un_use_lines添加新的LINE，可随时将un_use_lines里的元素整合到in_use_lines中，当LINE名与in_use_lines或un_use_lines
//  中的任何一个元素重复时报错
//2 管理员可向in_use_lines或un_use_lines中的任意LINE中添加站点。每次添加站点都会对正在被添加的LINE进行站点名查询，若发现同名站点则报错，
//  若在in_use_lines中添加站点，则每次添加站点还会进行额外搜索以判断新添加的站点是否被有意设为交叉站点，并发出警告
//3 管理员在线路L执行删除站点S操作时若S为交叉站点，则应只分离L与S的关系，其他线路不受影响

//在解除两相邻站点之间的联系前需判断这两个站点是否至少同属于两条线路，若此种情况发生，则不应解除它们之间的联系，而只
//剔除掉待删除站点的所属线路关系
