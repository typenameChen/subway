//查找结果类
//负责集成查找结果信息，描述查找到的对象类型和该对象数量
#include "find.h"

FIND::FIND(const string&tp, int c):type(tp),count(c){}

std::string FIND::Type() const
{
    return type;
}

int FIND::Count() const
{
    return count;
}

FIND::~FIND()
{

}

