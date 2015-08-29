#include "serialacmanager.h"
#include"serialac.h"
#include<iostream>
#include<stdio.h>
#include "ztool.h"
#include<QTimer>
#include<QFile>
 #include <QEventLoop>
#include<QThread>
#include <unistd.h>
#include<QDebug>
using namespace std;
static void QextSerialPort_init(QextSerialPort *port)
{
    port->setDtr(false);
    port->setBaudRate(BAUD19200);
    port->setFlowControl(FLOW_OFF);
    port->setParity(PAR_NONE);
    port->setDataBits(DATA_8);
    port->setStopBits(STOP_1);
    port->setTimeout(300);

}
void SerialACManager::setDirection(int d)
{
    QString io = QString::number(_dirIO);
    if(_dirIO == 0)
        return;

    QFile f("/sys/class/gpio/gpio"+io+"/value");
    if(!f.open(QFile::ReadWrite))
        qDebug("open file value failed!!");
	f.write(QString::number(d).toAscii());
    f.close();
    sync();
}
SerialACManager::SerialACManager(const QString& devName,int dirIO,QueryMode mode)
{
    _dirIO = dirIO;
    QString io = QString::number(dirIO);
    if(0 != _dirIO)
    {
        if(!QFile::exists("/sys/class/gpio/gpio"+io))
        {
            QString cmd = "echo "+io+" > /sys/class/gpio/export";
            system(cmd.toAscii().data());
            QFile f;
            if(!QFile::exists("/sys/class/gpio/gpio13"))
                qDebug("not exits gpio13");
            f.setFileName("/sys/class/gpio/gpio"+io+"/direction");
            f.open(QFile::ReadWrite);
            f.write("out");
            f.close();
            sync();
            setDirection(0);
        }
    }
    if(mode == EventDriven)
    {
        serial = new QextSerialPort(devName,QextSerialPort::EventDriven);
        connect(serial,SIGNAL(readyRead()),this,SLOT(readyData()));
    }
    else if(mode == Polling)
        serial = new QextSerialPort(devName,QextSerialPort::Polling);
    QextSerialPort_init(serial);

}

SerialACManager::~SerialACManager()
{
    delete serial;
}
void SerialACManager::readyData()
{
    static QString buff;
    QByteArray tmp;
    int start = -1,end = -1;
    while((tmp = serial->readAll()).length() != 0)
        buff.append(tmp);
    if(start == -1)
    {
        start = buff.indexOf(':');
        if(start == -1)
            return;
    }
    if(end == -1)
    {
        end = buff.indexOf("\x0d\x0a",start);
        if(end == -1)
            return;
    }
    SerialAC* ac = new SerialAC(buff.mid(start,end-start+2).toAscii());
    buff.clear();
    acList.append(ac);
    start = -1;
    end = -1;

    emit readyRead();

}
SerialAC* SerialACManager::getOneAc()
{
    if(acList.isEmpty())
        return NULL;
    SerialAC* ac = acList.front();
    acList.pop_front();
    return ac;
}//返回动态生成对象 ，使用后要delete  失败返回NULL

SerialAC* SerialACManager::WaitOneAc(int msecs)
{
    QEventLoop q;
    SerialAC* resAc = NULL;
    QTimer::singleShot(msecs,&q,SLOT(quit()));
    connect(this,SIGNAL(readyRead()),&q,SLOT(quit()));
    q.exec();
    resAc = getOneAc();
    return resAc;
}
void SerialACManager::SendOneAc(SerialAC& ac,int ms)
{
    setDirection(1);
    ac.generate();
    serial->write(ac.rawData);
    ZTool::msleep(ms);
    setDirection(0);

}
bool SerialACManager::open()
{
    bool res = serial->open(QIODevice::ReadWrite);
    if(!res)
        qDebug()<<serial->errorString();
    return res;
}
