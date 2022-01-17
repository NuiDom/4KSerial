#include "upgradePIC.h"
#include <QStringList>
#include <stdint.h>
#include "extserial.h"
#include <QDebug>
#include <QFile>
#include <QByteArray>
#include <QStringRef>

QString fileName = "/home/dnutt/Desktop/output.bin";
ExtSerial Serial;
upgradePIC::upgradePIC(QObject *parent) : QObject(parent)
{

}

upgradePIC::~upgradePIC()
{

}

int upgradePIC::ProgramPIC()
{
    int 		SynqTries;

                qDebug() << "\r\nPIC Upgrade!";


                QFile file(fileName);

                if (!file.open(QIODevice::ReadOnly)){
                    qDebug() << "Unable to open file :"<< fileName;
                    return -1;
                    }

                qDebug() << "File open";
                QByteArray BINFile = file.readAll();
                int FileSize = BINFile.size();

//                for(SynqTries=0;SynqTries<5;SynqTries++)
//                {
//                    Port->clear_port();
//                    Port->write_port("BOOTLOADER");
//                    Port->read_port(100);
//                    qDebug() << "BOOTLOADER CMD Reply" << Port->EXT_COMMAND;

//                    if(Port->EXT_COMMAND.left(2)=="bl")
//                        break;
//                }

//                if(SynqTries==5)
//                {
//                    emit signalUpgradeDone(DONE_STATUS,"Error communicating with board! ");
//                    return -1;
//                }

                //----------------------------- Flash Programming ----------------------------
                //----------------------------------------------------------------------------

                int     FilePosition=0;
                int     n32AddressCounter=0x00;
                char 	szBuffer[20];
                char 	szAddress[20];
                int 	Length;
                int     Progress=0;

                int Max = FileSize/64;


                qDebug() <<"Programming Flash...";

//                do{
                QByteArray Block64;
                for(int a=0; a<3; a++){
                    Block64 = BINFile.mid(FilePosition,64);
                    Serial.clear_port();
                    Serial.write_port_bin(Block64, 64);
                    qDebug() << Block64;
                    FilePosition+=64;
                    msdelay(500);
                    Serial.read_port(3000);
                    if(Serial.EXT_COMMAND.left(2)!="64")
                    {
                        emit signalUpgradeDone(DONE_STATUS,"Error Writing to Flash! ");

                        qDebug()<< "Error writing to Flash!";
                        return -1;
                    }
                    qDebug() << "BOOTLOADER CMD Reply" << Serial.EXT_COMMAND;
                }


//            }while(FilePosition-64 < FileSize);
            qDebug() <<"Programming Flash...Done";

    return(0);
}
