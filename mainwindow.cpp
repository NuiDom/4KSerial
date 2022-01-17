#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "extserial.h"
#include <QtCore>
#include <QtGui>
#include <upgradePIC.h>

extern ExtSerial Serial;
upgradePIC Upgrade;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(signalProgramPIC()), &Upgrade, SLOT(ProgramPIC()));
//    ui->progressBarUpgrade->setValue(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    Serial.open_port("/dev/ttyACM0",B115200);
}


void MainWindow::on_pushButton_3_clicked()
{
    Serial.close_port();
}


void MainWindow::on_pushButton_2_clicked()
{
    Serial.clear_port();
    Serial.write_port("1");
    qDebug() << "1";
}


void MainWindow::on_pushButton_4_clicked()
{
    Serial.open_port("/dev/ttyACM0",B115200);
    Serial.write_port("BOOTLOADER");
    msdelay(50);
    Serial.close_port();
    qDebug() << "BOOTLOADER";
}


void MainWindow::on_pushButton_5_clicked()
{
    Serial.write_port("JUMP");
    qDebug() << "JUMP";
    Serial.close_port();
}


void MainWindow::on_Upgrade_PushButton_clicked()
{
    emit signalProgramPIC();
}

