#ifndef SERIALACMANAGER_H
#define SERIALACMANAGER_H

#include <QThread>
#include "qextserialport/src/qextserialport.h"
#include<QList>
#include"serialac.h"
class SerialACManager : public QObject
{
    Q_OBJECT
        QextSerialPort *serial;
        QList<SerialAC*> acList;

signals:
        void readyRead();
public:
    enum QueryMode {
        Polling,
        EventDriven
    };
    enum ResultState
    {
        SUCCESS,
        TIMEOUT,
        FAILED
    };
    SerialAC* getOneAc();//返回动态生成对象 ，使用后要delete  失败返回NULL
    SerialAC* WaitOneAc(int timeout = 300);//failed return NULL
    void SendOneAc(SerialAC& ac,int ms = 0);

    SerialACManager(const QString& devName,int  dirIO = 0,QueryMode mode = EventDriven);
    bool open();
    void close(){serial->close();}
    ~SerialACManager();
private:
    int bytesWrittenCnt;
    int _dirIO;
    void setDirection(int d); //485方向　0收1发
private slots:
    void onByteWritten(){bytesWrittenCnt++;}
public slots:
    void readyData();
};

#endif // SERIALACMANAGER_H

