#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <QString>

class ACCOUNT//用户的帐号与密码
{
public:
    QString account;//帐号
    QString password;//密码
    ACCOUNT(const QString&acc,const QString&pass);
    bool operator==(const ACCOUNT&acc)const;
    bool operator!=(const ACCOUNT&acc)const;
    ~ACCOUNT();
};

#endif // ACCOUNT_H
