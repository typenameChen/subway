//查找结果类
//负责集成查找结果信息
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

