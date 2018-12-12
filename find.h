//查找结果类  负责集成查找结果信息，描述查找到的对象类型和数量
#ifndef FIND_H
#define FIND_H
#include <string>
using std::string;

class FIND
{
private:
    string type;
    int count;
public:
    FIND(const string&tp="",int c=0);
    string Type()const;
    int Count()const;
    ~FIND();
};

#endif // FIND_H
