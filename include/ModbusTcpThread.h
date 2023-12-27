#ifndef MODBUSTHREAD_H
#define MODBUSTHREAD_H


#include <QString>
#include <QThread>
#include <QTimer>
#include <QTime>

#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>

/////////////////////////// 2.Fins
class OmronFinsTcpThread: public QThread
{
    Q_OBJECT

public:
    explicit OmronFinsTcpThread(QObject *parent = 0);

    ~OmronFinsTcpThread();

    void calc_systemInfo(bool isShow);

    void writeResult(int address, int value);

    QTcpSocket* _socket;

    bool _isRunning;
    bool _isRecvResult;
    int _result;

    bool _isSendHeartbeat;
    bool _isShowSystemInfo;

//    QTimer* _timer;
    bool _heartbeat;

    int _cnt_disconnect;
    int _cnt_lossHeart;

protected:
     void run();

signals:
     void emit_startTrigger(int value);
     void emit_PLCstate(int value);
     void emit_callbackCamid(int camid);

     void emit_log(QString text);


private slots:
     QTcpSocket* reconnect();

     void sendHandShake();
     int sendHeartbeat(bool heartbeat);
     char readDataFromPLC(int address);

public:
    void writeDataToPLC(int address, int value);


};


#endif // MODBUSTHREAD_H
