#include "ModbusTcpThread.h"
#include "videopanel.h"
#include <iostream>
#include <stdio.h>
#include "sys/time.h"

#define  MODBUS_IP "192.168.10.1"  //"192.168.250.1"
#define  MODBUS_PORT 502

#define  LOCAL_IP "192.168.250.10"
#define  FINS_IP  "192.168.250.12"  //"192.168.250.1"
#define  FINS_PORT 9600

using namespace std;

#define PLC_OK 0
//////////////////////////// 2.
OmronFinsTcpThread::OmronFinsTcpThread(QObject *parent) : QThread(parent)
{
//    this->setParent(parent);
    _isRunning = true;
    _isRecvResult = false;
    _result = -1;

    _isSendHeartbeat = true;
    _isShowSystemInfo = true;
    _heartbeat = false;

    _cnt_disconnect = 0;
    _cnt_lossHeart = 0;

//    _timer = new QTimer(this);
//    connect(_timer, &QTimer::timeout, this, [=](){_isSendHeartbeat = true;});
//    _timer->start(1000);
}

OmronFinsTcpThread::~OmronFinsTcpThread()
{
//    _socket->disconnectFromHost();
    _socket->close();
    delete  _socket;
    _socket = NULL;
}

void OmronFinsTcpThread::calc_systemInfo(bool isShow)
{
    if (isShow){
        _isShowSystemInfo = true;
    }
    else{
        _isShowSystemInfo = false;
    }
}

void OmronFinsTcpThread:: writeResult(int address, int value)
{
    if (address == 0x03){

        _isRecvResult = true;
        _result = value;
    }
}

QTcpSocket* OmronFinsTcpThread::reconnect()
{
    // 连接到 Omron PLC 的 IP 和端口
    _socket = new QTcpSocket();
    _socket->connectToHost(QHostAddress(FINS_IP), FINS_PORT); // 替换为你的 PLC IP 和端口
    qDebug() << "start connect >>>>   " << FINS_IP << ":" << FINS_PORT;
    QString text = "";
    if (_socket->waitForConnected(1500)){
        text = "[OmronFinsTcpThread] Fins TCP 连接成功.\n";

        sendHandShake();
        emit emit_log(text);
        qDebug() << text;

        return _socket;
    }
    else{
        text = "[OmronFinsTcpThread] Fins TCP 连接失败!!!\n";

        emit emit_log(text);
        qDebug() << text;
        return NULL;
    }
}

/////////////////////////
void OmronFinsTcpThread::run()
{
    // 连接到 Omron PLC 的 IP 和端口
    _socket = this->reconnect();
    if (_socket == NULL){
        QMessageBox* mgBox = newMessageBox(NULL, "错误", "PLC连接失败！");
        mgBox->exec();

        return;
    }


//    connect(_socket, &QTcpSocket::readyRead, this, &OmronFinsTcpThread::recv_readyRead);

//    writeDataToPLC(0x0E, 1);
//    printf("[OmronFinsTcpThread] 0x05  设备初始化\n");

    while (_isRunning)
    {
        if (_socket->state() != QAbstractSocket::ConnectedState){
            emit emit_log(QTime::currentTime().toString("hh:mm:ss.zzz") + "  PLC连接断开");
            if (_cnt_disconnect >= 50){  // 500 ms
                _cnt_disconnect = 0;

                _socket->close();
                _socket->disconnectFromHost();
                _socket = this->reconnect();
                if (_socket == NULL){
                    emit emit_log(QTime::currentTime().toString("hh:mm:ss.zzz") + "  PLC重连失败！！！");
                    QMessageBox* mgBox = newMessageBox(NULL, "错误", " PLC重连失败！！！ ");
                    mgBox->exec();

                    return;
                }
                else{
                    emit emit_log(QTime::currentTime().toString("hh:mm:ss.zzz") + "  PLC重连成功！！！");
                }
            }
            _cnt_disconnect++;

            msleep(10);
            continue;
        }

        // 0x03: 写入结果, PLC会自动写0
        if (_isRecvResult){
            _isRecvResult = false;

            this->writeDataToPLC(0x03, _result);
            _result = 0;
//            this->writeDataToPLC(0x03, 0);
        }
        else{
            // 0x02 到位信号
            if (readDataFromPLC(0x02) == '1'){
                writeDataToPLC(0x02, 0);
                emit emit_PLCstate(2);
                //            printf("[OmronFinsTcpThread] 0x02  到位信号\n");
            }

//            // 0x01 心跳
//            if (_isSendHeartbeat){
//                int ret = sendHeartbeat(_heartbeat);
//                if (ret != -1){
//                    _isSendHeartbeat = false;
//                    emit emit_log(QTime::currentTime().toString("hh:mm:ss.zzz") + "  PLC发送心跳...");

//                    emit emit_PLCstate(1);
//                    _heartbeat = !_heartbeat;
////                    printf("[OmronFinsTcpThread] 0x01  心跳  ret=%d\n", ret);
//                }
//                else{
//                    emit emit_log(QTime::currentTime().toString("hh:mm:ss.zzz") + "  PLC发送心跳失败！！！");
//                    if (_socket->state() != QAbstractSocket::ConnectedState){
//                        emit emit_log(QTime::currentTime().toString("hh:mm:ss.zzz") + "  PLC连接断开");
//                        _cnt_lossHeart++;
//                        if (_cnt_lossHeart >= 20){ // 200 ms
//                            _cnt_lossHeart = 0;

//                            _socket->close();
//                            _socket->disconnectFromHost();
//                            _socket = this->reconnect();
//                            if (_socket == NULL){
//                                emit emit_log(QTime::currentTime().toString("hh:mm:ss.zzz") + "  PLC重连失败！！！");
//                                QMessageBox* mgBox = newMessageBox(NULL, "错误", " PLC重连失败！！！ ");
//                                mgBox->exec();

//                                return;
//                            }
//                        }
//                    }
//                }

//            }
            QThread::msleep(10);

        }
    }
}


///////
void OmronFinsTcpThread::sendHandShake()
{
    QByteArray readHand;
    readHand.append(char(0x46));
    readHand.append(char(0x49));
    readHand.append(char(0x4E));
    readHand.append(char(0x53));

    readHand.append(char(0x00));
    readHand.append(char(0x00));
    readHand.append(char(0x00));
    readHand.append(char(0x0C));

    readHand.append(char(0x00));
    readHand.append(char(0x00));
    readHand.append(char(0x00));
    readHand.append(char(0x00));

    readHand.append(char(0x00));
    readHand.append(char(0x00));
    readHand.append(char(0x00));
    readHand.append(char(0x00));

    readHand.append(char(0x00));
    readHand.append(char(0x00));
    readHand.append(char(0x00));
    readHand.append(char(0x0A));  // ip client

    int ret = -1;
    ret = _socket->write(readHand);
    _socket->flush();

    printf("[OmronFinsTcpThread] 握手  ret=%d\n", ret);
}

///////
int OmronFinsTcpThread::sendHeartbeat(bool heartbeat)
{
    // 构建读取地址0x01的FINS报文（心跳包）
    QByteArray readHeartbeat;

    readHeartbeat.append(char(0x46));
    readHeartbeat.append(char(0x49));
    readHeartbeat.append(char(0x4E));
    readHeartbeat.append(char(0x53));

    readHeartbeat.append(char(0x00));
    readHeartbeat.append(char(0x00));
    readHeartbeat.append(char(0x00));
    readHeartbeat.append(char(0x1C));

    readHeartbeat.append(char(0x00));
    readHeartbeat.append(char(0x00));
    readHeartbeat.append(char(0x00));
    readHeartbeat.append(char(0x02));

    readHeartbeat.append(char(0x00));
    readHeartbeat.append(char(0x00));
    readHeartbeat.append(char(0x00));
    readHeartbeat.append(char(0x00));

    readHeartbeat.append(char(0x80)); // ICF
    readHeartbeat.append(char(0x00)); // RSV
    readHeartbeat.append(char(0x02)); // GCT:目标网络号
    readHeartbeat.append(char(0x00)); // DNA

    readHeartbeat.append(char(0x0C)); // DA1: ip server
    readHeartbeat.append(char(0x00)); // DA2
    readHeartbeat.append(char(0x00)); // SNA
    readHeartbeat.append(char(0x0A)); // SA1: ip client

    readHeartbeat.append(char(0x00)); // SA2
    readHeartbeat.append(char(0xFF)); // SID
    readHeartbeat.append(char(0x01)); // MRC
    readHeartbeat.append(char(0x02));

    readHeartbeat.append(char(0x82)); // Memory Area
    readHeartbeat.append(char(0x00)); // DM Address
    readHeartbeat.append(char(0x01)); ///// DM Adress
    readHeartbeat.append(char(0x00)); // number of DM Words

    readHeartbeat.append(char(0x00));
    readHeartbeat.append(char(0x01));
    readHeartbeat.append(char(0x00));
    if (heartbeat)
        readHeartbeat.append(char(0x00)); // Data 0
    else
        readHeartbeat.append(char(0x01)); // Data 1
    // 读取地址0x01的心跳包数据
    int ret = -1;
    ret = _socket->write(readHeartbeat);
    _socket->flush();

     return ret;
}


// write result ---- addr:0x03  value:  OK  1, NG  2,  null  3
// write alarm  ---- addr:0x04  value:  正常 1;  异常  2(unload etc..)
void OmronFinsTcpThread::writeDataToPLC(int address, int value)
{
    // 构建读取地址0x0*的FINS报文（心跳包）
    QByteArray writeData;

    writeData.append(char(0x46));
    writeData.append(char(0x49));
    writeData.append(char(0x4E));
    writeData.append(char(0x53));

    writeData.append(char(0x00));
    writeData.append(char(0x00));
    writeData.append(char(0x00));
    writeData.append(char(0x1C));

    writeData.append(char(0x00));
    writeData.append(char(0x00));
    writeData.append(char(0x00));
    writeData.append(char(0x02));

    writeData.append(char(0x00));
    writeData.append(char(0x00));
    writeData.append(char(0x00));
    writeData.append(char(0x00));

    writeData.append(char(0x80)); // ICF
    writeData.append(char(0x00)); // RSV
    writeData.append(char(0x02)); // GCT
    writeData.append(char(0x00)); // DNA

    writeData.append(char(0x0C)); // DA1  ip PLC server
    writeData.append(char(0x00)); // DA2
    writeData.append(char(0x00)); // SNA
    writeData.append(char(0x0A)); // SA1  ip client

    writeData.append(char(0x00)); // SA2
    writeData.append(char(0x00)); // SID
    writeData.append(char(0x01)); //
    writeData.append(char(0x02)); /////////

    writeData.append(char(0x82)); // Memory Area
    writeData.append(char(0x00)); // DM Address
    writeData.append(char(address)); ///// DM Adress
    writeData.append(char(0x00)); // number of DM Words

    writeData.append(char(0x00));
    writeData.append(char(0x01));
    writeData.append(char(0x00));
    writeData.append(char(value)); // Data 1

    // 写入数据到PLC
    int ret = -1;
    ret = _socket->write(writeData);
    _socket->flush();

    if (address == 0x03){
//        if (value == 1 || value == 2)
        printf("#######  0x03  写入汇总结果: %d  ret=%d  %s\n\n", value, ret, QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss.zzz").toLatin1().data());;
    }
    else if (address == 0x04){
        printf("#######  0x04  相机掉线!\n\n");
    }
    else if (address == 0x05){
        printf("#######  0x05  计数清零!\n\n");
    }

}

/////////////
char OmronFinsTcpThread::readDataFromPLC(int address)
{
    // 构建读取地址0x0*的FINS报文
    QByteArray readData;

    readData.append(char(0x46));
    readData.append(char(0x49));
    readData.append(char(0x4E));
    readData.append(char(0x53));

    readData.append(char(0x00));
    readData.append(char(0x00));
    readData.append(char(0x00));
    readData.append(char(0x1A));

    readData.append(char(0x00));
    readData.append(char(0x00));
    readData.append(char(0x00));
    readData.append(char(0x02));

    readData.append(char(0x00));
    readData.append(char(0x00));
    readData.append(char(0x00));
    readData.append(char(0x00));

    readData.append(char(0x80)); // ICF
    readData.append(char(0x00)); // RSV
    readData.append(char(0x02)); // GCT:目标网络号
    readData.append(char(0x00)); // DNA

    readData.append(char(0x0C)); // DA1: ip PLC server
    readData.append(char(0x00)); // DA2
    readData.append(char(0x00)); // SNA
    readData.append(char(0x0A)); // SA1: ip client

    readData.append(char(0x00)); // SA2
    readData.append(char(0x00)); // SID
    readData.append(char(0x01)); // MRC
    readData.append(char(0x01)); ///////// SRC

    readData.append(char(0x82)); // Memory Area
    readData.append(char(0x00)); // DM Address
    readData.append(char(address)); ///// DM Adress
    readData.append(char(0x00)); // number of DM Words

    readData.append(char(0x00));
    readData.append(char(0x01));

    // 读取数据从PLC
    int ret = -1;
    ret = _socket->write(readData);
    _socket->flush();

    _socket->waitForBytesWritten();
    _socket->waitForReadyRead();
    QByteArray data = _socket->readAll();
    QString sc_plcResult=QString(data.toHex().toUpper());

//    qDebug() << "read plc_data:" << sc_plcResult <<"   value=" << sc_plcResult[63].toLatin1();

    return sc_plcResult[63].toLatin1();

}



