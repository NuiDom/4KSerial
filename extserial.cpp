#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <QString>
#include <QDebug>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <QThread>
#include <QElapsedTimer>
#include <QtCore>

#include "extserial.h"

ExtSerial::ExtSerial(QObject *parent) :
        QObject(parent)
{
    EXT_COMMAND_RECIEVED=false;
    EXT_COMMAND="\0";
//    EXT_COMMAND="";
    nBufferPosition=0;
}

ExtSerial::~ExtSerial()
{

}

/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */

int ExtSerial::open_port(const char *PortName, speed_t baud)
{
    struct termios tty;
    struct termios tty_old;

    fd = open(PortName, O_RDWR| O_NOCTTY | O_SYNC);
    if(fd > 0){
        qDebug() << "Port open" << PortName;
        fcntl(fd, F_SETFL, 0);
        memset (&tty, 0, sizeof tty);

        /* Error Handling */
        if ( tcgetattr ( fd, &tty ) != 0 ) {
           qDebug() << "Error " << errno << " from tcgetattr: " << strerror(errno);
        }
        /* Save old tty parameters */
        tty_old = tty;

        /* Set Baud Rate */
        cfsetospeed (&tty, baud);
        cfsetispeed (&tty, baud);

        /* Setting other Port Stuff */
        tty.c_cflag     &=  ~PARENB;            // Make 8n1
        tty.c_cflag     &=  ~CSTOPB;
        tty.c_cflag     &=  ~CSIZE;
        tty.c_cflag     |=  CS8;

        tty.c_cflag     &=  ~CRTSCTS;           // no flow control
        tty.c_cc[VMIN]   =  1;                  // read doesn't block
        tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
        tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

        /* Make raw */
        cfmakeraw(&tty);

        /* Flush Port, then applies attributes */
        tcflush( fd, TCIFLUSH );
        if ( tcsetattr ( fd, TCSANOW, &tty ) != 0) {
           qDebug() << "Error " << errno << " from tcsetattr";
        }
    }
    else
        qDebug() << "Unable to open PORT " << PortName;


      return (fd);
}

int ExtSerial::close_port(void)
{
    qDebug() << "Port Closed";
    return close(fd);
}

int ExtSerial::write_port(QString Buffer)
{
    int nCMDLength = Buffer.count();

    char B[100];

    strcpy(B,Buffer.toStdString().c_str());

    //qDebug() << Buffer.toStdString().c_str(); // qPrintable(Buffer);
    return write (fd, B, nCMDLength);
}

int ExtSerial::write_port_bin(QByteArray Buffer, int nCMDLength)
{
int nBytesWritten;


  //  this->clear_port();
     // Write Command to port
    nBytesWritten=write (fd, Buffer, nCMDLength);

    // Error checking
    if(nBytesWritten!=nCMDLength)
        qDebug() << "Error writing to serial port: Bytes Written = " << nCMDLength;
#ifdef SERIAL_DEBUG
    else
        qDebug() << "Bytes Written= " << nBytesWritten;
#endif

    return(0);
}

int ExtSerial::read_port(int Timeout)
{
    int nBytesRead=0;
    int nBytesAvailiablel;
    char CharRecieved=0;
    QElapsedTimer timeoutTimer;
    timeoutTimer.start();
    bool finished = false;

    if(fd > 0)
    {
        do {// Check if there is data to read
            ioctl(fd, FIONREAD, &nBytesAvailiablel);
            if(nBytesAvailiablel>=1)
            {
                nBytesRead = read( fd, &CharRecieved, 1 );

                if(CharRecieved != '\n'){
                    EXT_COMMAND = EXT_COMMAND + QString(CharRecieved); //[nBufferPosition] = CharRecieved;
                    //EXT_COMMAND[nBufferPosition+1] = 0;
                    //nBufferPosition += nBytesRead;
                    }
                }

                if(CharRecieved == '\n'){
                    finished = true;
                }

        } while( (CharRecieved != '\n' && (nBytesAvailiablel > 1)) || (timeoutTimer.elapsed() < Timeout && !finished ));  // '\r' is the terminator


        if(CharRecieved=='\n'){ // '\r' received
            EXT_COMMAND_RECIEVED=true;
//            qDebug() << "EXTCOMMAND: " << EXT_COMMAND;
        }
    }
    else
    {
        return -1;
    }




    return(nBytesRead);
}



void ExtSerial::clear_port()
{
    /* System calls, removed for now */
//    ioctl(fd,TCIOFLUSH,0);
//    ioctl(fd,TCIOFLUSH,1);

    // Check if there is data in the buffer
    unsigned long nBytesAvailiable=0;
    ioctl(fd, FIONREAD, &nBytesAvailiable);
    uint8_t Buffer[1000];
    if(nBytesAvailiable > 1000){ // Added to safely clear if buffer is extremly large
        qDebug() << "Warning!! Buffer exceeds 1000..";
        while (nBytesAvailiable > 1000) {
            read( fd, &Buffer, 1000);
            ioctl(fd, FIONREAD, &nBytesAvailiable);
        }
    }
    else if(nBytesAvailiable>=1){
        read( fd, &Buffer, nBytesAvailiable );
    }

    EXT_COMMAND = QString(); //intializes the string
    nBufferPosition =0;
    EXT_COMMAND_RECIEVED=false;
}

void msdelay(int msec)
{
    QEventLoop loop;

    QTimer::singleShot(msec, &loop, &QEventLoop::quit);

    loop.exec();
}
