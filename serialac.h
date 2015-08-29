#ifndef SERIALAC_H
#define SERIALAC_H
#include<QDateTime>
#include<QList>
#include<ztool.h>
//呼叫状态解析器
class SerialAC
{
public:

    int addr;
    int code;//功能码



    int regAddr;
    QList<int> datList;
    QByteArray rawData;
    int datCount;//数据数量
    bool isValid; //数据是否有效
//    public:
//    bool hasCall1;//呼叫1是否发生
//    bool hasCall2;//呼叫2是否发生
//    bool hasCall3;//呼叫3是否发生
//    bool hasCall4;//呼叫4是否发生
//    bool hasBroadcast;//是否有广播信号

private:
    int LRC;

    quint8 calCLR();//接收时候调用
    void load(QByteArray pkg);


    quint8 generateCLR();

public:
    SerialAC();
    void generate();
    explicit SerialAC(QByteArray pkg){load(pkg);}
    void addDat(int dat){datList.append(dat);}

};

#endif // SERIALAC_H
