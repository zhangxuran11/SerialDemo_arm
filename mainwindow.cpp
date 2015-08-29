#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ztool.h"
#include<unistd.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    char str[10] = "";
    qDebug("%llx",ZTool::ascii2hex("a5f0ed",3));
    qDebug("%s",ZTool::hex2ascii(0xfa5f0,str,3));
    acm = new SerialACManager("/dev/ttymxc1",13,SerialACManager::EventDriven);
    connect(acm,SIGNAL(readyRead()),this,SLOT(fun()));
    on_pushButton_clicked();
}
void MainWindow::fun()
{
    SerialAC* ac = acm->getOneAc();
    if(ac!= NULL)
    {
        QString text;
        text = QString::number(ac->addr)+" "+QString::number(ac->code)+" "+QString::number(ac->datCount)+" "
                +" "+QString::number(ac->isValid)+"datList :";
        for(int i = 0;i< ac->datList.length();i++)
            text += " "+ac->datList[i];
        text+="\n";
        ui->textEdit->textCursor().insertText(text);
    }
    else
        ui->textEdit->textCursor().insertText("invalid acm");
}
MainWindow::~MainWindow()
{
    delete acm;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(acm->open())
        ui->pushButton->setText("on");
    else
        qDebug("open acm failed");
}

void MainWindow::on_pushButton_2_clicked()
{
    acm->close();
    ui->pushButton->setText("off");
}

void MainWindow::on_pushButton_3_clicked()
{
    SerialAC ac;
    ac.addr = 1;
    ac.code = 0x10;
    ac.regAddr = 0x04;
    ac.datCount = 4;
    ac.addDat(1);
    ac.addDat(0);
    ac.addDat(0);
    ac.addDat(0x3F21);
    ac.generate();
    acm->SendOneAc(ac,19200.0/11/1000*ac.rawData.length());
}
