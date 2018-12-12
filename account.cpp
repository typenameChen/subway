#include "account.h"

ACCOUNT::ACCOUNT(const QString&acc,const QString&pass):account(acc),password(pass){}

bool ACCOUNT::operator==(const ACCOUNT &acc)const
{
    return account==acc.account&&password==acc.password;
}

bool ACCOUNT::operator!=(const ACCOUNT &acc)const
{
    return account!=acc.account||password!=acc.password;
}

ACCOUNT::~ACCOUNT()
{

}

