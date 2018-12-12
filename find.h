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
