#ifndef EXTSERIAL_H
#define EXTSERIAL_H

#include <stdint.h>
#include <QString>
#include <QObject>
#include <QTimer>
#include <QByteArray>
#include <termios.h>

class ExtSerial : public QObject
{
    Q_OBJECT

public:

    ExtSerial(QObject *parent = 0);
    ~ExtSerial();

    int open_port(const char *,speed_t baud = B115200);
    int close_port(void);
    int write_port(QString);
    int write_port_bin(QByteArray Buffer, int nCMDLength);
    int read_port(int);
    int read_lines(int);
    void clear_port(void);

    bool EXT_COMMAND_RECIEVED;
    QString EXT_COMMAND;
    int nBufferPosition;

public slots:


signals:

    void CMDRecieved(void);

private:

      int fd; /* File descriptor for the port */
};

#endif // EXTSERIAL_H
