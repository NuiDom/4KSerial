#ifndef UPGRADEPIC_H
#define UPGRADEPIC_H

#include <extserial.h>
#include <QObject>

#define INFO_STATUS 0
#define DONE_STATUS 1

extern ExtSerial Serial;

class upgradePIC : public QObject
{
    Q_OBJECT
public:
     explicit upgradePIC(QObject *parent = 0);
    ~upgradePIC();

public slots:
    int	ProgramPIC();

signals:
    void SetProgressBar(int,int);
    void signalUpgradeDone(int,QString);


public:
    ExtSerial *Port;
    QString szFileName;
};

#endif // UPGRADEPIC_H
