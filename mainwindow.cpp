#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "extserial.h"

ExtSerial Serial;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Serial.open_port("/dev/ttyACM0",B115200);
    qDebug() << "Hello World";
}

MainWindow::~MainWindow()
{
    delete ui;
}

