#include "widget.h"
#include <QApplication>
#include "subway.h"
#include <QString>

#include "testhead.h"//test
void ShowVec(const vector<string>&strs)
{
    for(const string&str:strs)
        cout<<str<<' ';
    cout<<endl;
}

int main(int argc, char *argv[])
{
#if 1
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
#endif
#if 0
    SUBWAY subway;
    try
    {
        subway.CreatLineToUnUse("CHEN");
        (subway.GetLine("CHEN",UN_USE))<<"1111"<<"2222"<<"3333"<<"4444"<<"5555"<<"6666";
        subway.MoveLineToInUse("CHEN");

        subway.CreatLineToUnUse("LONG");
        (subway.GetLine("LONG",UN_USE))<<"0000"<<"2222"<<"3333"<<"4444"<<"7777"<<"8888";
        subway.MoveLineToInUse("LONG");



        vector<PATH_MSG>path_msgs=subway.GetTopPaths("1111","8888",5,PASS);
//        qDebug()<<path_msgs.size();
//        ShowVec(subway.GetLine("LONG",IN_USE).Find("4444")->blt_line);
        for(const PATH_MSG&pm:path_msgs)
        {
            cout<<"Path: ";
            ShowVec(pm.GetPath());
            cout<<"Tr_count: "<<pm.GetTransferTimes()<<endl;
            cout<<"Ps_count: "<<pm.GetPassCount()<<endl;
            cout<<"Tr_point: ";
            ShowVec(pm.GetTransferPoint());
            cout<<"Lines_order: ";
            ShowVec(pm.GetLinesOrder());
        }

    }
    catch(string err_str)
    {
        qDebug()<<err_str.c_str();
    }

    return 0;
#endif
}
