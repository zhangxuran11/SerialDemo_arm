#include<ztool.h>
#include"serialac.h"
#include "qextserialport/src/qextserialport.h"
#include<stdio.h>

quint8 SerialAC::calCLR()
{
    quint8 sum = 0;
    sum = addr + code;
    if(code == 0x10)
    {
        sum += (regAddr >> 8)+regAddr + datCount;
    }
    else if(code == 0x03)
    {
        sum += datCount << 1;
        for(int i = 0;i<datCount;i++)
            sum += (datList[i] >> 8) +datList[i];
    }

    return quint8(~sum + 1);
}
SerialAC::SerialAC()
{
    regAddr = 0;
    isValid = false;
}

void SerialAC::load(QByteArray pkg)
{
    regAddr = 0;
    isValid = false;
    datList.clear();
    char* off = pkg.data();
    addr = ZTool::ascii2hex(off+1,2);
    code = ZTool::ascii2hex(off+3,2);

    if(code == 0x03)
    {
        datCount = ZTool::ascii2hex(off+5,2) >> 1;
        for(int i = 0;i < datCount;i++)
            datList.append(ZTool::ascii2hex(off+7+(i<<2),4));
        LRC = ZTool::ascii2hex(off+7+(datCount<<2),2);
    }
    else if(code == 0x10)
    {
        regAddr = ZTool::ascii2hex(off+5,4);
        datCount = ZTool::ascii2hex(off+9,4);
        LRC = ZTool::ascii2hex(off+13,2);
    }
    if(datCount != datList.length())
        return;
    isValid = LRC == calCLR();

}

quint8 SerialAC::generateCLR()
{
    quint8 sum = 0;
    sum = addr +code+(regAddr>>8) + regAddr + (datCount>>8)+datCount;
    if(code == 0x10)
    {
        sum += (datCount<<1);
        for(int i = 0;i < datCount;i++)
            sum+=(datList[i]>>8)+datList[i];
    }
    sum = ~sum + 1;
    return sum;
}

void SerialAC::generate()
{
    isValid = false;
    char tmp[10];
    rawData.clear();
    rawData.append(':');
    rawData.append(ZTool::hex2ascii(addr,tmp,2),2);
    rawData.append(ZTool::hex2ascii(code,tmp,2),2);
    rawData.append(ZTool::hex2ascii(regAddr,tmp,4),4);
    rawData.append(ZTool::hex2ascii(datCount,tmp,4),4);
    if(code == 0x10)
    {
        rawData.append(ZTool::hex2ascii(datCount<<1,tmp,2),2);
        for(int i = 0;i < datCount;i++)
            rawData.append(ZTool::hex2ascii(datList[i],tmp,4),4);
    }
    LRC = generateCLR();
    rawData.append(ZTool::hex2ascii(LRC,tmp,2),2);
    rawData.append(0x0d);
    rawData.append(0x0a);
    isValid = true;

}

