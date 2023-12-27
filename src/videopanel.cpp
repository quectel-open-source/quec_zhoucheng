#include "videopanel.h"
#include "ModbusTcpThread.h"
#include "DaHengCamera/GxIAPI.h"
#include "DaHengCamera/DxImageProc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fstream>
#include <iostream>
#include <semaphore.h>

#include "qevent.h"
#include "qmenu.h"
#include "qlayout.h"
#include "qlabel.h"
#include "qtimer.h"
#include "qdebug.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"

#include <exception>
#include <stdlib.h>
#include <iostream>
#include <QtConcurrent>
#include <QMetaType>
#include <QMessageBox>

using namespace std;

#define WORK_OK 0
#define WORK_NG 1
#define WORK_NULL 2

#define ShowLabelNum 7     // 7个显示label
#define AlgoNum 5          // 4个
#define CameraRealNum 5    // 5个相机

#define WorkerNum 7        // 5个相机工位 + 1翻转 + 1擦拭
#define CameraNum 7        // 5个相机 + 1擦拭 +1翻转

CamDevice*  vcamdevice[ShowLabelNum];
std::ofstream _logFile;

typedef struct CamPicNodeEach
{
    int result;           /**< OK  0, NG  1,  null  2, */
    QString SN;
}CamPicNodeEach;
typedef struct CamPicNode
{
    int result;           /**< OK  1, NG  2,  null  3, */
    QString SN;
}CamPicNode;
//工位和相机关系;反向是因为8号相机最接近要推出的轴承
int wcRelation[CameraNum] = {6,5,4,3,2,1,0}; // 多了1擦拭  {6,5,5,4,3,2,2,1,0};

CamPicNodeEach _CameraEachpushArray[CameraNum];
CamPicNode _CamResultArray[WorkerNum][CameraNum];
int _result_cam2 = 0;
int _result_cam4 = 0;

extern bool _isShowAllLabel;  // 显示每张图的label, true: 1   false: 0
CamRusultRecv _labels_1_show = CamRusultRecv();
CamRusultRecv _labels_4_show = CamRusultRecv();

//指向位置就是要出队列的位置
int raIndex = 0;
int resultLen = 0; //结果长度

QList<int> _CamPushNums = {0,0,0,0,0,0,0};

bool _isPushInit = true;
int _numOK = 0;
int _numNG = 0;
int _numTotal = 0;
int _numTotalAll = 0;
int _numPush = 0;
int _numDaoWei = 0;
QList<int> _numNGTypes = {0, 0, 0, 0, 0, 0, 0};

QMap<int, QString> sqlResShow = {{0, "OK"}, {1, "NG"}, {2, "NULL"}, {3, "不均匀"}, {4, "碰伤"}, {5, "脏污"}, {6, "毛糙"}, {7, "无盖"}, {8, "凹坑"}, {9, "划痕"}};
int sqlPicCol[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8,9};
bool _isCameraError = false;
bool _isPLCError = false;

QList<int> _camPicCnts_eachPush = {1, 2, 3, 3, 3, 4, 5};
QList<int> _camResultStates;                  // 每个相机
QList<int> _camResultStates_tmp;                  // 每个相机
int _camCallbackCnts[CameraRealNum] = {0};   // 每个相机触发次数

extern QString _cpp2Dir; // = "/home/quectel/AIQt/aiqt/zhouCheng2"; // "/workspace/AIQt/aiqt/zhouCheng2";
extern QString _saveDir;
extern QMap<QString, QString> _settingJsons;

int _nextAction = -1;
bool _isAllEnd = true;

///////
int VideoPanel::onePush(QString timeCur)
{
    //按工位和相机关系写入子结果
    for (int i = 0;i < CameraNum;i++) {
        if (i == 3 || i == 4)  // 1翻转 +1初始空
            continue;

        int loc = (raIndex + wcRelation[i]) % WorkerNum;
        _CamResultArray[loc][i].result = _CameraEachpushArray[i].result;
        _CamResultArray[loc][i].SN = _CameraEachpushArray[i].SN;
    }

    int result = WORK_OK;
    //当前循环队列长度小于工位数5才增加，等于工位数5就循环出队列
    resultLen++;
    if (resultLen >= WorkerNum) {
        resultLen = WorkerNum;

        int isNG = 1;
        //结果汇总:9个相机子结果全OK,则总体OK; 9个相机子结果全NULL,则总体NULL; 如果中间状态，则NG
        for (int i=0; i< CameraNum; i++) {
            if (i == 3 || i == 4)
                continue;

            if (_CamResultArray[raIndex][i].result == WORK_NULL){
                result = WORK_NULL;
                break;
            }
            else if (_CamResultArray[raIndex][i].result == WORK_NG) {
                isNG = 0;
            }
        }

        if (result != WORK_NULL){
            if (isNG == 0){
                result = WORK_NG;
            }
        }

        // 1. 发送Modbus信号  ---- addr:x03  value:  OK  1, NG  2,  null  3
        if (result == WORK_NULL){
            _nextAction = WORK_NULL;
        }
        else if(result == WORK_OK){
            _numOK++;
            _nextAction = WORK_OK;
        }
        else{
            _numNG++;
            _nextAction = WORK_NG;
        }
        _numTotal = _numOK + _numNG;
        printf("****** [%s] result=%d,  numTotal=%d,  numOK=%d,  numNG=%d ******\n", __func__, result, _numTotal, _numOK, _numNG);

        // 2 把结果插入数据库
        printf("[%s] sql insert: start\n", __func__);
        bool isInsert = query.exec(
                QString("insert into workpieces(SN,cam1,cam2,cam3,cam4,cam5,result,timeCur,location,numOK,numNG,numTotal,numTotalAll) "
                        "values ('%1','%2','%3','%4','%5','%6','%7','%8','%9',%10,%11,%12,%13)")
                .arg(_CamResultArray[raIndex][0].SN)
                .arg(sqlResShow[_CamResultArray[raIndex][0].result]).arg(sqlResShow[_CamResultArray[raIndex][1].result])
                .arg(sqlResShow[_CamResultArray[raIndex][2].result]).arg(sqlResShow[_CamResultArray[raIndex][5].result])
                .arg(sqlResShow[_CamResultArray[raIndex][6].result])
                .arg(sqlResShow[result]).arg(timeCur)
                .arg(_settingJsons["save_date_dir"]).arg(_numOK).arg(_numNG).arg(_numTotal).arg(_numTotalAll)
                );
        if (isInsert){
            printf("[%s] sql insert: success!!!\n", __func__);
        }
        else {
            printf("[%s] sql insert: error!!!\n", __func__);
        }
    }
    // 3. 在下一个节拍, 才push汇总结果, 发送Modbus信号
    if(_nextAction==WORK_OK){
        emit emit_writeResultOrOffline(0x03, 1);
        emit emit_sendui(_numOK, _numNG, _numTotal, _numNGTypes);
    }
    else if (_nextAction==WORK_NG){
        emit emit_writeResultOrOffline(0x03, 2);
        emit emit_sendui(_numOK, _numNG, _numTotal, _numNGTypes);
    }
    else{
        emit emit_writeResultOrOffline(0x03, 3);
    }
    _nextAction = -1;

    //移动其实位置就相当于出列了
    raIndex=(raIndex+1) % WorkerNum;

    //下一个节拍进入前，把工位相机图像位清零
    for (int i = 0;i < CameraNum;i++) {
        _CameraEachpushArray[i].result = 0;
        _CameraEachpushArray[i].SN = "";
    }
    return result;

}


//////
VideoPanel::VideoPanel(QWidget *parent) : QWidget(parent)
{
    QString logTXT = _cpp2Dir + QString("/log/%1.txt").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss"));  // yyyyMMdd_hhmmsszzz
    _logFile.open(logTXT.toStdString(), std::ios::app);

    this->initForm();

    num=0;
    _timeTotal = 0;
    _initCnts = 0;
    _initTimes = 0.0f;
    _heartbeatCnts = 0;

#ifdef USING_MODBUS
    // Modbus
    _modbusTcpThread = new ModbusTcpThread(this);
    connect(this, &VideoPanel::emit_writeResultOrOffline, _modbusTcpThread, &ModbusTcpThread::Write);
    connect(_modbusTcpThread, &ModbusTcpThread::emit_startTrigger, this, &VideoPanel::modbusTcpThread_PLCstate_received);
    connect(_modbusTcpThread, &ModbusTcpThread::emit_PLCstate, this, &VideoPanel::modbusTcpThread_PLCstate_received);
    connect(_modbusTcpThread, &ModbusTcpThread::emit_callbackCamid, this, &VideoPanel::modbusTcpThread_callbackCamid_received);
#else
    // Fins
    _modbusTcpThread = new OmronFinsTcpThread(this);
    connect(this, &VideoPanel::emit_writeResultOrOffline, _modbusTcpThread, &OmronFinsTcpThread::writeResult);
    connect(_modbusTcpThread, &OmronFinsTcpThread::emit_startTrigger, this, &VideoPanel::modbusTcpThread_PLCstate_received);
    connect(_modbusTcpThread, &OmronFinsTcpThread::emit_PLCstate, this, &VideoPanel::modbusTcpThread_PLCstate_received);
    connect(_modbusTcpThread, &OmronFinsTcpThread::emit_callbackCamid, this, &VideoPanel::modbusTcpThread_callbackCamid_received);
    connect(_modbusTcpThread, &OmronFinsTcpThread::emit_log, this, &VideoPanel::mainThread_log_received);
#endif
    _modbusTcpThread->start();

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &VideoPanel::initClear_and_onePush);


    _timer_calcSysteInfo = new QTimer(this);
    connect(_timer_calcSysteInfo, &QTimer::timeout, this, &VideoPanel::timer_timeout_calcSysteInfo);
    _is_getAllResults = true;
#ifdef TEST
    connect(_timer, &QTimer::timeout, this, &VideoPanel::test_timeout_daoWei);

//    for(int i=0; i<7; i++)
//    {
//        QTimer* timer = new QTimer(this);
//        _timers_software.append(timer);
//    }
//    connect(_timers_software[0], &QTimer::timeout, this, &VideoPanel::test_timeout_software_1);
//    connect(_timers_software[1], &QTimer::timeout, this, &VideoPanel::test_timeout_software_2);
//    connect(_timers_software[2], &QTimer::timeout, this, &VideoPanel::test_timeout_software_3);
//    connect(_timers_software[3], &QTimer::timeout, this, &VideoPanel::test_timeout_software_4);
//    connect(_timers_software[4], &QTimer::timeout, this, &VideoPanel::test_timeout_software_5);
//    connect(_timers_software[5], &QTimer::timeout, this, &VideoPanel::test_timeout_software_6);
//    connect(_timers_software[6], &QTimer::timeout, this, &VideoPanel::test_timeout_software_7);
#endif
    _isStart_rightMenu = false;
}

VideoPanel::~VideoPanel()
{
    for (int i = 0; i < ShowLabelNum; ++i)
    {
        delete vcamdevice[i];
    }
}

void VideoPanel::loadHistoryResult()
{
//    query.exec(QString("select SN,numOK,numNG,numTotal from workpieces where timeCur = {select MAX(timeCur) from workpieces}"));
    query.exec(QString("select SN,numOK,numNG,numTotal from workpieces order by cast(SN as integer) desc limit 1 "));
//    query.exec(QString("select SN,numOK,numNG,numTotal from workpieces order by SN desc limit 1 "));
    int SN = 0;
    while(query.next()) {
        SN        = query.value(0).toString().toInt();
        _numOK    = query.value(1).toInt();
        _numNG    = query.value(2).toInt();
        _numTotal = query.value(3).toInt();
        emit emit_sendui(_numOK, _numNG, _numTotal, _numNGTypes);

        _CamPushNums[0] = _numTotal;
        _CamPushNums[1] = _numTotal;
        _CamPushNums[2] = _numTotal;
        _CamPushNums[3] = _numTotal;
        _CamPushNums[4] = _numTotal;
        printf(">>>>>> History: SN=%d, OK=%d, NG=%d, total=%d\n", SN, _numOK, _numNG, _numTotal);
    }

    this->JudgeCameraState(vcamdevice[3]->m_imvCamera->displayGigeDeviceInfo());

    this->updateCameraState();

//    emit emit_writeResultOrOffline(0x05, 4);

    this->mainThread_initChar_received(2,  vcamdevice[1]->m_mainThread->_charDir_all.keys());
}

bool VideoPanel::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        QLabel *label = (QLabel *) watched;
        if (!_isLabelMax) {
            _isLabelMax = true;

            hide_allLabels();
            this->layout()->addWidget(label);
            label->setVisible(true);
        }
        else {
            _isLabelMax = false;

            show_allLabels();
        }
        label->setFocus();
    }
    else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = (QMouseEvent *)event;
        if (mouseEvent->button() == Qt::RightButton) {
            videoMenu->exec(QCursor::pos());
        }
    }

    return QWidget::eventFilter(watched, event);
}

void VideoPanel::show_allLabels()
{
    for (int i = 0; i < _widgets.count(); i++)
    {
        _widgets[i]->setVisible(true);
    }
//    updateGridLayout(_layout, _widgets, {2, 2, 3}, {1, 1, 1, 1});
    updateGridLayout_SiYuan(_layout, _widgets);
}

void VideoPanel::hide_allLabels()
{
    for (int i = 0; i < _widgets.count(); i++)
    {
        _layout->removeWidget(_widgets[i]);
        _widgets[i]->setVisible(false);
    }
}

void VideoPanel::start_closed()
{
    for (int i = 0; i < ShowLabelNum; ++i)
    {
        vcamdevice[i]->m_mainThread->closed();
    }
}

////////////////
void VideoPanel::mainThread_log_received(QString text)
{
    _logFile << text.toStdString() << std::endl;
}

void VideoPanel::mainThread_initState(int initState, float loaded_time)
{
    if (initState < 0){
        emit emit_inferState(-1);
    }
    else if (initState == 0){
        emit emit_inferState(0);
    }
    else{
        _initCnts += initState;
        _initTimes += loaded_time;
        if (_initCnts == AlgoNum){
            emit emit_inferState(1);

            QString info = QString("< Load %1 Models average-time: %2 ms >").arg(AlgoNum).arg(_initTimes / (float)AlgoNum);
            this->mainThread_log_received(info);
            qDebug() << info;
        }
    }
}


////////////////
void VideoPanel::update_preParams(int camid, QList<int> pre_params)
{
    switch (camid)
    {
    case 1:
        vcamdevice[0]->m_mainThread->transform_preParams(1, pre_params);
        vcamdevice[0]->m_mainThread->transform_preParams(5, pre_params);
        break;
    case 2:
        vcamdevice[1]->m_mainThread->transform_preParams(2, pre_params);
        vcamdevice[1]->m_mainThread->transform_preParams(4, pre_params);
        vcamdevice[2]->m_mainThread->transform_preParams(2, pre_params);
        vcamdevice[5]->m_mainThread->transform_preParams(4, pre_params);
        break;
    case 3:
        vcamdevice[3]->m_mainThread->transform_preParams(3, pre_params);
        break;
    }
}

void VideoPanel::update_preROI(int camid, QList<int> pre_ROI)
{
    switch (camid)
    {
    case 1:
        vcamdevice[0]->m_mainThread->transform_preROI(1, pre_ROI);
        break;
//    case 2:
//        vcamdevice[1]->m_mainThread->transform_preROI(2, pre_ROI);
//        vcamdevice[2]->m_mainThread->transform_preROI(2, pre_ROI);
//        break;
    case 3:
        vcamdevice[3]->m_mainThread->transform_preROI(3, pre_ROI);
        break;
    }
}

void VideoPanel::update_postParams(int camid, QList<QString> label_list, QMap<QString, QList<int>> labels_params)
{
    switch (camid)
    {
    case 1:
        vcamdevice[0]->m_mainThread->transform_postParams(1, labels_params);
        break;
    case 2:
    {
        QMap<QString, QList<int>> labels_params_0, labels_params_1;
        int cnts = label_list.count();
        for(int i=0; i<cnts; i++)
        {
            if (i == cnts - 1){
                labels_params_1[label_list[i]] = labels_params[label_list[i]];
            }
            else{
                labels_params_0[label_list[i]] = labels_params[label_list[i]];
            }
        }

        vcamdevice[1]->m_mainThread->transform_postParams(22, labels_params_0);
        vcamdevice[2]->m_mainThread->transform_postParams(2, labels_params_1);
        vcamdevice[5]->m_mainThread->transform_postParams(2, labels_params_1);
        break;
    }
    case 3:
        vcamdevice[3]->m_mainThread->transform_postParams(3, labels_params);
        break;
    }
}


//////////////// 显示
void VideoPanel::mainThread_results_1_received(int camid, int campicid, QImage dstImage, int result, QString SN, float area_one)
{
    if (camid == 1){
        _CameraEachpushArray[0].result = result;
        _CameraEachpushArray[0].SN = SN;

        if (_numDaoWei >= 2)
            emit emit_onePush(1);

//        vcamdevice[0]->mlabel->clear();
        vcamdevice[0]->mlabel->setPixmap(QPixmap::fromImage(dstImage));
    }
}
void VideoPanel::mainThread_results_5_received(int camid, int campicid, QImage dstImage, int result, QString SN, float area_one)
{
    if (camid == 5){
        _CameraEachpushArray[6].result = result;
        _CameraEachpushArray[6].SN = SN;

        if (_numDaoWei >= 8)
            emit emit_onePush(5);

//        vcamdevice[6]->mlabel->clear();
        vcamdevice[6]->mlabel->setPixmap(QPixmap::fromImage(dstImage));
    }
}

void VideoPanel::mainThread_results_2_received(int camid, int campicid, QImage dstImage, int result, QString SN, float sum_area)
{
    if (camid == 2){
        if (campicid == 1){
            _result_cam2 = result;
//            vcamdevice[1]->mlabel->clear();
            vcamdevice[1]->mlabel->setPixmap(QPixmap::fromImage(dstImage));
        }
        else if (campicid == 5){
            int res = (result== 0 && _result_cam2 == 0) ? 0 : 1;
            _CameraEachpushArray[1].result = res;
            _CameraEachpushArray[1].SN = SN;
            _result_cam2 = 0;

            if (_numDaoWei >= 3)
                emit emit_onePush(2);
//            emit emit_init_photo_store();

//            vcamdevice[2]->mlabel->clear();
            vcamdevice[2]->mlabel->setPixmap(QPixmap::fromImage(dstImage));
        }
    }
}
void VideoPanel::mainThread_results_4_received(int camid, int campicid, QImage dstImage, int result, QString SN, float area_one)
{
    if (camid == 4){
        if (campicid == 1){
            _result_cam4 = result;
//            vcamdevice[4]->mlabel->clear();
            vcamdevice[4]->mlabel->setPixmap(QPixmap::fromImage(dstImage));
        }
        else if (campicid == 5){
            int res = (result== 0 && _result_cam4 == 0) ? 0 : 1;
            _CameraEachpushArray[5].result = res;
            _CameraEachpushArray[5].SN = SN;
            _result_cam4 = 0;

            if (_numDaoWei >= 7)
                emit emit_onePush(4);
//            emit emit_init_photo_store();

//            vcamdevice[5]->mlabel->clear();
            vcamdevice[5]->mlabel->setPixmap(QPixmap::fromImage(dstImage));
        }
    }
}

void VideoPanel::mainThread_results_3_received(int camid, int campicid, QImage dstImage, int result, QString SN, float area_one)
{
    _CameraEachpushArray[2].result = result;
    _CameraEachpushArray[2].SN = SN;

    if (_numDaoWei >= 4)
        emit emit_onePush(3);

//    vcamdevice[3]->mlabel->clear();
    vcamdevice[3]->mlabel->setPixmap(QPixmap::fromImage(dstImage));
}


////////////////
void VideoPanel::initForm()
{
    // 1. 设置样式表
    QStringList qss;
    qss.append("QFrame{border:2px solid #000000;}");
    qss.append("QLabel{font:75 25px;color:#F0F0F0;border:2px solid #AAAAAA;background:#303030;}");
    qss.append("QLabel:focus{border:2px solid #00BB9E;background:#555555;}");
    this->setStyleSheet(qss.join(""));

    for (int i = 0; i < ShowLabelNum; i++)
    {
        vcamdevice[i] = new CamDevice;
        vcamdevice[i]->mlabel = new QLabel(this);
        vcamdevice[i]->mlabel->setObjectName(QString("video%1").arg(i + 1));
        vcamdevice[i]->mlabel->installEventFilter(this);
        vcamdevice[i]->mlabel->setFocusPolicy(Qt::StrongFocus);
        vcamdevice[i]->mlabel->setAlignment(Qt::AlignCenter);
        vcamdevice[i]->mlabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        vcamdevice[i]->mlabel->setFrameShape(QFrame::Box);
        vcamdevice[i]->mlabel->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
        vcamdevice[i]->mlabel->setProperty("video", true);

        vcamdevice[i]->mlabel->setScaledContents(true);
        vcamdevice[i]->mlabel->setPixmap(QPixmap("/workspace/cpp2/quec_libs/platforms/mask.jpg"));
        _widgets.append(vcamdevice[i]->mlabel);

        if (i == 3){
            vcamdevice[i]->m_imvCamera = new DaHuaCamera(this->parentWidget()->parentWidget());
            // test: no use
            connect(vcamdevice[i]->m_imvCamera, &DaHuaCamera::emit_callbackCamid, this, &VideoPanel::modbusTcpThread_callbackCamid_received);

        }
        else{
            vcamdevice[i]->m_gxCamera = new GXCamera(this->parentWidget()->parentWidget());
            // test: no use
            connect(vcamdevice[i]->m_gxCamera, &GXCamera::emit_callbackCamid, this, &VideoPanel::modbusTcpThread_callbackCamid_received);

        }
//        vcamdevice[i]->m_mvsCamera = new MVSCamera(this);
        vcamdevice[i]->id = i+1;
//        vcamdevice[i]->m_hWnd=(void *)vcamdevice[i]->mlabel->winId();
        vcamdevice[i]->m_v=this;
    }
    this->initMenu();
    this->parentWidget()->parentWidget()->show();

    connect(this, &VideoPanel::emit_onePush, this, &VideoPanel::set_onePush);

#ifdef USING_MVS_CAMERA
    connect(this, &VideoPanel::emit_cam1_pushNum, vcamdevice[0]->m_mvsCamera, &MVSCamera::set_pushNum);
    connect(this, &VideoPanel::emit_cam1_pushNum, vcamdevice[3]->m_mvsCamera, &MVSCamera::set_pushNum);
    connect(this, &VideoPanel::emit_cam2_pushNum, vcamdevice[1]->m_mvsCamera, &MVSCamera::set_pushNum);
    connect(this, &VideoPanel::emit_cam3_pushNum, vcamdevice[2]->m_mvsCamera, &MVSCamera::set_pushNum);

    connect(this, &VideoPanel::emit_cam4_pushNum, vcamdevice[4]->m_mvsCamera, &MVSCamera::set_pushNum);
    connect(this, &VideoPanel::emit_cam4_pushNum, vcamdevice[7]->m_mvsCamera, &MVSCamera::set_pushNum);
    connect(this, &VideoPanel::emit_cam5_pushNum, vcamdevice[5]->m_mvsCamera, &MVSCamera::set_pushNum);
    connect(this, &VideoPanel::emit_cam6_pushNum, vcamdevice[6]->m_mvsCamera, &MVSCamera::set_pushNum);

    connect(this, &VideoPanel::emit_cam7_pushNum, vcamdevice[8]->m_mvsCamera, &MVSCamera::set_pushNum);
#else
    connect(this, &VideoPanel::emit_cam1_pushNum, vcamdevice[0]->m_gxCamera, &GXCamera::set_pushNum);
    connect(this, &VideoPanel::emit_cam2_pushNum, vcamdevice[1]->m_gxCamera, &GXCamera::set_pushNum);
//    connect(this, &VideoPanel::emit_cam2_pushNum, vcamdevice[2]->m_gxCamera, &GXCamera::set_pushNum);

    connect(this, &VideoPanel::emit_cam3_pushNum, vcamdevice[3]->m_imvCamera, &DaHuaCamera::set_pushNum);

    connect(this, &VideoPanel::emit_cam4_pushNum, vcamdevice[4]->m_gxCamera, &GXCamera::set_pushNum);
    connect(this, &VideoPanel::emit_cam5_pushNum, vcamdevice[6]->m_gxCamera, &GXCamera::set_pushNum);
//    connect(this, &VideoPanel::emit_cam5_pushNum, vcamdevice[6]->m_gxCamera, &GXCamera::set_pushNum);
#endif


    this->initForm_setSaveDir();
#ifdef USING_MVS_CAMERA
    this->initForm_MVSCameras();
#else
    this->initForm_GxCamera();
#endif
}


void VideoPanel::initForm_MVSCameras()
{
    printf("--------initial camera and inference-------\n");

    qRegisterMetaType<ImageInfo>("ImageInfo");
    qRegisterMetaType<Bearing_input_data>("Bearing_input_data");
    qRegisterMetaType<CamRusultRecv>("CamRusultRecv");
    qRegisterMetaType<QList<QString>>("QList<QString>");

    for (int i = 0; i < ShowLabelNum; ++i)
    {
        switch (i)
        {
        case 0:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_%2.json").arg(_settingJsons["model_jsonDir"]).arg(1), true/*false*/, false, 7, 4);
            connect(vcamdevice[0]->m_mainThread, &MainThread::emit_initChar, this, &VideoPanel::mainThread_initChar_received);
            break;
        case 1:
        case 2:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_%2.json").arg(_settingJsons["model_jsonDir"]).arg(i+1), true, false, 13, 13);
            break;
        case 3:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_%2.json").arg(_settingJsons["model_jsonDir"]).arg(1), true, true, 7, 4);
            break;
        case 4:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_%2.json").arg(_settingJsons["model_jsonDir"]).arg(1), true/*false*/, false, 7, 4);
            connect(vcamdevice[4]->m_mainThread, &MainThread::emit_initChar, this, &VideoPanel::mainThread_initChar_received);
            break;
        case 5:
        case 6:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_%2.json").arg(_settingJsons["model_jsonDir"]).arg(i-3), true, false, 13, 13);
            break;
        case 7:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_%2.json").arg(_settingJsons["model_jsonDir"]).arg(1), true, true, 7, 4);
            break;
        case 8:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_%2.json").arg(_settingJsons["model_jsonDir"]).arg(i-1), true, false, 22, 22);
            break;
        }


//        connect(vcamdevice[i]->m_gxCamera, &MVSCamera::emit_log, this, &VideoPanel::mainThread_log_received);
//        connect(vcamdevice[i]->m_gxCamera, &MVSCamera::emit_imageInfo, vcamdevice[i]->m_mainThread, &MainThread::transform_data);
//        connect(vcamdevice[i]->m_mainThread, &MainThread::emit_initYes, this, &VideoPanel::mainThread_initState);
//        connect(vcamdevice[i]->m_mainThread, &MainThread::emit_log, this, &VideoPanel::mainThread_log_received);
////        connect(vcamdevice[i]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_received);
////        vcamdevice[i]->m_mainThread->start();
    }

//    _thread7_2 = new MainThread(this, 9, QString("%1/model_7.json").arg(_settingJsons["model_jsonDir"]), true, false, 22, 22);
//    connect(vcamdevice[8]->m_gxCamera, &MVSCamera::emit_imageInfo, _thread7_2, &MainThread::transform_data);
//    connect(_thread7_2, &MainThread::emit_log, this, &VideoPanel::mainThread_log_received);
////    _thread7_2->start();

//    connect(vcamdevice[0]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_1_received);
//    connect(vcamdevice[3]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_1_7_received);
//    connect(vcamdevice[1]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_2_received);
//    connect(vcamdevice[2]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_3_received);

//    connect(vcamdevice[4]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_4_received);
//    connect(vcamdevice[7]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_4_7_received);
//    connect(vcamdevice[5]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_5_received);
//    connect(vcamdevice[6]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_5_received);

//    connect(vcamdevice[8]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_7_received);
//    connect(_thread7_2, &MainThread::emit_results, this, &VideoPanel::mainThread_results_7_received);


//    connect(vcamdevice[0]->m_gxCamera, &MVSCamera::emit_imageInfo, vcamdevice[3]->m_mainThread, &MainThread::transform_data);
//    connect(vcamdevice[4]->m_gxCamera, &MVSCamera::emit_imageInfo, vcamdevice[7]->m_mainThread, &MainThread::transform_data);


//    connect(vcamdevice[0]->m_mainThread, &MainThread::emit_input_data, vcamdevice[3]->m_mainThread, &MainThread::mainThread_input_data_received);
//    connect(vcamdevice[4]->m_mainThread, &MainThread::emit_input_data, vcamdevice[7]->m_mainThread, &MainThread::mainThread_input_data_received);


//    connect(this, &VideoPanel::emit_init_photo_store, vcamdevice[3]->m_mainThread, &MainThread::mainThread_init_photo_store_received);
//    connect(this, &VideoPanel::emit_init_photo_store, vcamdevice[7]->m_mainThread, &MainThread::mainThread_init_photo_store_received);


//    connect(vcamdevice[0]->m_mainThread, &MainThread::emit_labels_show, vcamdevice[3]->m_mainThread, &MainThread::transform_labels_show);
//    connect(vcamdevice[3]->m_mainThread, &MainThread::emit_labels_show, vcamdevice[0]->m_mainThread, &MainThread::transform_labels_show);

//    connect(vcamdevice[4]->m_mainThread, &MainThread::emit_labels_show, vcamdevice[7]->m_mainThread, &MainThread::transform_labels_show);
//    connect(vcamdevice[7]->m_mainThread, &MainThread::emit_labels_show, vcamdevice[4]->m_mainThread, &MainThread::transform_labels_show);

//    connect(vcamdevice[8]->m_mainThread, &MainThread::emit_labels_show, _thread7_2, &MainThread::transform_labels_show);
////    connect(_thread7_2, &MainThread::emit_labels_show, vcamdevice[8]->m_mainThread, &MainThread::transform_labels_show);


    // 2. 初始化相机
    int nRet = MV_OK;
    MV_CC_DEVICE_INFO_LIST  m_stDevList;
    memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    // ch:枚举子网内所有设备 | en:Enumerate all devices within subnet
    nRet = MVSCamera::EnumDevices(MV_GIGE_DEVICE, &m_stDevList);
    if (MV_OK != nRet){
        printf("<<< Enum devices fail! >>>\n");
    }

    if (m_stDevList.nDeviceNum <= 0){
        printf("<<< Find No Devices! >>>\n");
    }

    //    this->JudgeCameraState(m_stDevList.nDeviceNum);
    printf("########### Camera Nums: %d ###########\n", m_stDevList.nDeviceNum);

    for(int j=0; j < m_stDevList.nDeviceNum; j++)
    {
        printf("############################################\n");
        std::cout << "device: " << j << std::endl;
        MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[j];
        if (NULL == pDeviceInfo){
            _camStates.append(j);
            _camStates.append("192.168." + QString::number(j+1) +".10");
            _camStates.append(-1);

            continue;
        }

        QString ipStr;
        int net;
//        vcamdevice[j]->m_gxCamera->PrintDeviceInfo(pDeviceInfo, net, ipStr);

        printf("  %s  device_id=%d\n", ipStr.toLatin1().data(), j);

        _camStates.append(net-1);
        _camStates.append(ipStr);
//        if (net < 4)
//            _camStates.append(vcamdevice[net-1]->m_gxCamera->initCamera(net, pDeviceInfo));
//        else if (net >= 4 && net < 7)
//            _camStates.append(vcamdevice[net]->m_gxCamera->initCamera(net, pDeviceInfo));
//        else
//            _camStates.append(vcamdevice[net+1]->m_gxCamera->initCamera(net, pDeviceInfo));
    }

    printf("##################################################\n");
    printf("--------initial completed-------\n");

    //    int counts = std::count(states.begin(), states.end(), 0);
    //    this->JudgeCameraState(counts);
    //    });

}

///////////
void VideoPanel::initForm_GxCamera()
{
    printf("--------initial camera and inference-------\n");

    qRegisterMetaType<ImageInfo>("ImageInfo");
    qRegisterMetaType<Bearing_input_data>("Bearing_input_data");
    qRegisterMetaType<CamRusultRecv>("CamRusultRecv");
    qRegisterMetaType<QList<QString>>("QList<QString>");

    for (int i = 0; i < ShowLabelNum-1; i++)
    {
        switch (i)
        {
        case 0:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_1.json").arg(_settingJsons["model_jsonDir"]), true, false, 1, 1);
            break;
        case 1:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_2.json").arg(_settingJsons["model_jsonDir"]), true, false, 5, 2);
            break;
        case 2:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_2.json").arg(_settingJsons["model_jsonDir"]), true, false, 5, 2);
            break;
        case 3:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_3.json").arg(_settingJsons["model_jsonDir"]), true, false/*true*/, 1, 1);
            break;
        case 4:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_2.json").arg(_settingJsons["model_jsonDir"]), false, false, 5, 2);
            break;
        case 5:
            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_2.json").arg(_settingJsons["model_jsonDir"]), true, false, 5, 2);
            break;
//        case 6:
//            vcamdevice[i]->m_mainThread = new MainThread(this, i, QString("%1/model_1.json").arg(_settingJsons["model_jsonDir"]), true, false, 1, 1);
//            break;
        }
        if(i == 3){
            connect(vcamdevice[i]->m_imvCamera, &DaHuaCamera::emit_log, this, &VideoPanel::mainThread_log_received);
//            connect(vcamdevice[i]->m_imvCamera, &DaHuaCamera::emit_imageInfo, vcamdevice[i]->m_mainThread, &MainThread::transform_data);
        }
        else{
            connect(vcamdevice[i]->m_gxCamera, &GXCamera::emit_log, this, &VideoPanel::mainThread_log_received);
//            connect(vcamdevice[i]->m_gxCamera, &GXCamera::emit_imageInfo, vcamdevice[i]->m_mainThread, &MainThread::transform_data);
        }
        connect(vcamdevice[i]->m_mainThread, &MainThread::emit_initYes, this, &VideoPanel::mainThread_initState);
        connect(vcamdevice[i]->m_mainThread, &MainThread::emit_log, this, &VideoPanel::mainThread_log_received);

        if (i != 4)
            vcamdevice[i]->m_mainThread->start();

    }

    connect(vcamdevice[1]->m_mainThread, &MainThread::emit_initChar, this, &VideoPanel::mainThread_initChar_received);

    connect(vcamdevice[0]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_1_received);
    connect(vcamdevice[0]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_5_received);
    connect(vcamdevice[1]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_2_received);
    connect(vcamdevice[1]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_4_received);
    connect(vcamdevice[2]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_2_received);
    connect(vcamdevice[5]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_4_received);
    connect(vcamdevice[3]->m_mainThread, &MainThread::emit_results, this, &VideoPanel::mainThread_results_3_received);

    connect(vcamdevice[0]->m_gxCamera, &GXCamera::emit_imageInfo, vcamdevice[0]->m_mainThread, &MainThread::transform_data);
    connect(vcamdevice[1]->m_gxCamera, &GXCamera::emit_imageInfo_others, vcamdevice[2]->m_mainThread, &MainThread::transform_data);
    connect(vcamdevice[3]->m_imvCamera, &DaHuaCamera::emit_imageInfo, vcamdevice[3]->m_mainThread, &MainThread::transform_data);

    connect(vcamdevice[1]->m_gxCamera, &GXCamera::emit_imageInfo, vcamdevice[1]->m_mainThread, &MainThread::transform_data);
    connect(vcamdevice[4]->m_gxCamera, &GXCamera::emit_imageInfo, vcamdevice[1]->m_mainThread, &MainThread::transform_data);
    connect(vcamdevice[4]->m_gxCamera, &GXCamera::emit_imageInfo_others, vcamdevice[5]->m_mainThread, &MainThread::transform_data);
    connect(vcamdevice[6]->m_gxCamera, &GXCamera::emit_imageInfo, vcamdevice[0]->m_mainThread, &MainThread::transform_data);

    connect(vcamdevice[4]->m_gxCamera, &GXCamera::emit_log, this, &VideoPanel::mainThread_log_received);
    connect(vcamdevice[6]->m_gxCamera, &GXCamera::emit_log, this, &VideoPanel::mainThread_log_received);
    connect(vcamdevice[1]->m_mainThread, &MainThread::emit_input_data, vcamdevice[2]->m_mainThread, &MainThread::mainThread_input_data_received);
    connect(vcamdevice[1]->m_mainThread, &MainThread::emit_input_data, vcamdevice[5]->m_mainThread, &MainThread::mainThread_input_data_received);

//    connect(this, &VideoPanel::emit_init_photo_store, vcamdevice[2]->m_mainThread, &MainThread::mainThread_init_photo_store_received);
    connect(this, &VideoPanel::emit_init_photo_store, vcamdevice[3]->m_mainThread, &MainThread::mainThread_init_photo_store_received);
//    connect(this, &VideoPanel::emit_startLine, this, &VideoPanel::getState);

    // 2. 初始化相机: daheng &&  dahua
    GX_STATUS       emStatus = GX_STATUS_SUCCESS;
    uint32_t ui32DeviceNum = 0;

    //Initialize libary
    emStatus = GXInitLib();
    if (emStatus != GX_STATUS_SUCCESS)
        GetErrorString(emStatus);

    //Get device enumerated number
    emStatus = GXUpdateDeviceList(&ui32DeviceNum, 1000);
    if (emStatus != GX_STATUS_SUCCESS){
        GetErrorString(emStatus);
        GXCloseLib();
    }

    //If no device found, app exit
    if (ui32DeviceNum <= 0){
        printf("----- <No device found> -----\n");
        GXCloseLib();
    }

    printf("########### Galaxy Camera Nums: %d ###########\n", ui32DeviceNum);
    vcamdevice[3]->m_imvCamera->displayGigeDeviceInfo(true);

    for(int j=1; j < CameraRealNum+1; j++)
    {
        printf("############################################\n");
        std::cout << "device: " << j << std::endl;
        _camStates.append(j);
        switch (j) {
        case 1:
            _camStates.append("192.168.1.100");
            _camStates.append(vcamdevice[0]->m_gxCamera->initCamera(1, 1));
            break;
        case 2:
            _camStates.append("192.168.22.100");
            _camStates.append(vcamdevice[1]->m_gxCamera->initCamera(2, 22)); // 2
            break;
        case 3:
            _camStates.append("192.168.3.100");
            _camStates.append(vcamdevice[3]->m_imvCamera->initCamera(3, "192.168.3.100"));

            break;
        case 4:
            _camStates.append("192.168.4.100");
            _camStates.append(vcamdevice[4]->m_gxCamera->initCamera(4, 4));
            break;
        case 5:
            _camStates.append("192.168.5.100");
            _camStates.append(vcamdevice[6]->m_gxCamera->initCamera(5, 5));  // 6
            break;
        }
    }

    printf("##################################################\n");
    printf("--------initial completed-------\n");


}



void VideoPanel::initForm_setSaveDir()
{
    QtConcurrent::run([=]{
        QString mergeDir = _settingJsons["save_merge_dir"];
        vcamdevice[0]->m_gxCamera->set_saveDir(_settingJsons["save_cam1_dir"], mergeDir);
        vcamdevice[1]->m_gxCamera->set_saveDir(_settingJsons["save_cam2_dir"], mergeDir);
//        vcamdevice[2]->m_gxCamera->set_saveDir(_settingJsons["save_cam2_dir"], mergeDir);

        vcamdevice[3]->m_imvCamera->set_saveDir(_settingJsons["save_cam3_dir"], mergeDir);

        vcamdevice[4]->m_gxCamera->set_saveDir(_settingJsons["save_cam4_dir"], mergeDir);
        vcamdevice[6]->m_gxCamera->set_saveDir(_settingJsons["save_cam5_dir"], mergeDir);
    });
}

void VideoPanel::updateCameraState()
{
    for(int i = 0; i< _camStates.count() / 3; i++)
    {
        emit emit_updateCamera(_camStates[i*3].toInt(), _camStates[i*3+1].toString(), _camStates[i*3+2].toInt());
    }
}

void VideoPanel::JudgeCameraState(int cnts)
{
    if (cnts < CameraRealNum){
        emit emit_writeResultOrOffline(0x04, 2);
        emit emit_cameraOffline(true);

        printf("-- 相机数目不对(%d/%d)，有相机掉线 --\n", cnts, CameraRealNum);
        QMessageBox* mgBox = newMessageBox(this, "错误", QString("\n 相机数目不对(%1/%2)，有相机掉线 \n").arg(cnts).arg(CameraRealNum));
        mgBox->exec();
    }
}

/////////
void VideoPanel::initMenu()
{
    _isLabelMax = false;
    _layout = newGridLayout_SiYuan(_widgets, 1, 1, {10,10,0,10});
//    _layout = newGridLayout(_widgets, {2, 2, 3}, {1, 1, 1, 1}, 1, 1, {10,10,0,10});
    this->setLayout(_layout);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    videoMenu = new QMenu(this);


    //单独关联信号槽
//    actionFull = new QAction("切换全屏模式", videoMenu);
//    connect(actionFull, SIGNAL(triggered(bool)), this, SLOT(full()));
//    actionPoll = new QAction("软 触发", videoMenu);
//    connect(actionPoll, SIGNAL(triggered(bool)), this, SLOT(rightMenu_startSoftware()));
//    actionTest = new QAction("测试 本地图像", videoMenu);
//    connect(actionTest, &QAction::triggered, this, &VideoPanel::test_imageDir);

    //通过QAction类方式添加子菜单
//    videoMenu->addAction(actionFull);
//    videoMenu->addSeparator();
//    videoMenu->addAction(actionPoll);
//    videoMenu->addSeparator();
//    videoMenu->addAction(actionTest);
//    videoMenu->addSeparator();

}
void VideoPanel::rightMenu_startSoftware()
{
    QDateTime time_daoWei = QDateTime::currentDateTime();
    emit emit_cam1_pushNum(_CamPushNums[0]++, 0, time_daoWei);
    emit emit_cam2_pushNum(_CamPushNums[1]++, 0, time_daoWei);
    emit emit_cam3_pushNum(_CamPushNums[2]++, 0, time_daoWei);
    emit emit_cam4_pushNum(_CamPushNums[3]++, 0, time_daoWei);
    emit emit_cam5_pushNum(_CamPushNums[4]++, 0, time_daoWei);
    emit emit_cam6_pushNum(_CamPushNums[5]++, 0, time_daoWei);
    emit emit_cam7_pushNum(_CamPushNums[6]++, 0, time_daoWei);

    int nRet;
    for(int j=0; j < CameraRealNum; j++)
    {
        nRet = -1;
        printf("0send software  id=%d\n", j);
        if(j==4){
            nRet = vcamdevice[4]->m_imvCamera->CommandExecute("TriggerSoftware");
        }
        else{
            nRet = vcamdevice[j]->m_gxCamera->CommandExecute(GX_COMMAND_TRIGGER_SOFTWARE);
        }
        printf("1send line0  ret=%d\n", nRet);
        printf("---------------------------\n");
    }

}


void VideoPanel::mainThread_initChar_received(int camid, QList<QString> keys)
{
    if (camid == 2){
        _charsInit = keys;
        emit emit_initChar(keys);
//        qDebug() << "222222222222222222222     " << _charsInit;
    }
}
void VideoPanel::update_chars(QString chars1, QString chars2)
{
    vcamdevice[1]->m_mainThread->init_OCR(chars1, chars2);
    vcamdevice[4]->m_mainThread->init_OCR(chars1, chars2);
}


void VideoPanel::init_nums()
{
    _numOK = 0;
    _numNG = 0;
    _numTotal = 0;
    _numTotalAll = 0;

    _CamPushNums[0] = 0;
    _CamPushNums[1] = 0;
    _CamPushNums[2] = 0;
    _CamPushNums[3] = 0;
    _CamPushNums[4] = 0;
    _numDaoWei = 0;
    _numPush = 0;
}

//////////////////////////////////////////
void VideoPanel::set_onePush(int camid)
{
    if (_camResultStates.contains(camid)){
        _camResultStates_tmp.append(camid);
    }
    else{
        _camResultStates.append(camid);

        if (_camResultStates.contains(2) && _camResultStates.contains(3) && _camResultStates.contains(4)){
            this->timer_timeout_calcSysteInfo();

            _timer_calcSysteInfo->start(2000);
        }

    }
}


void VideoPanel::initClear_and_onePush()
{
    if (_camResultStates.count() == _camPicCnts_eachPush[_numPush]){  // camPicCnts_eachPush = {1, 2, 3, 3, 3, 4, 5}
        QDateTime timeStart = QDateTime::currentDateTime();

        QString info = QString(">> yes  DaoWei:%1  recv results: [ ").arg(_numDaoWei);
        for (int yes_camid : _camResultStates){
            info += QString::number(yes_camid) + ", ";
        }
        info += " ] / " + QString::number(_camPicCnts_eachPush[_numPush]);
        this->mainThread_log_received(info);
        qDebug() << info;

        _camResultStates.clear();

        if (_camResultStates_tmp.count() > 0){
            _camResultStates = _camResultStates_tmp;
            _camResultStates_tmp.clear();
        }

        _numPush++;
        if (_numPush >= WorkerNum)
            _numPush = WorkerNum - 1;

        // 汇总
        QString timeCur = timeStart.toString("yyyy-MM-dd  hh:mm:ss.zzz");
        this->onePush(timeCur);

        QDateTime timeEnd = QDateTime::currentDateTime();
        this->mainThread_log_received(
                    "\n [Summary Yes] start: " + timeCur + "   end: " + timeEnd.toString("yyyy-MM-dd  hh:mm:ss.zzz") +
                    "   time： "+ QString::number(timeStart.msecsTo(timeEnd)) + " ms\n"
        );

    }
    else if (_camResultStates.count() > _camPicCnts_eachPush[_numPush]){
        QList<int> l_good, l_repeat;

        for (int value: _camResultStates){
            if(l_good.contains(value))
                l_repeat.append(value);
            else
                l_good.append(value);
        }
        _camResultStates = l_good;
        _camResultStates_tmp = l_repeat;

        this->initClear_and_onePush();
    }
    else if (_camResultStates.count() < _camPicCnts_eachPush[_numPush]){
        QDateTime timeStart = QDateTime::currentDateTime();

        QString info = QString(">> no  DaoWei:%1  recv results: [").arg(_numDaoWei);
        for (int yes_camid : _camResultStates){
            info += QString::number(yes_camid) + ", ";
        }
        info += "] / " + QString::number(_camPicCnts_eachPush[_numPush]);
        this->mainThread_log_received(info);
        qDebug() << info;

        if (!_camResultStates.contains(1))
            _CameraEachpushArray[0].result = WORK_NG;
        else if (!_camResultStates.contains(2))
            _CameraEachpushArray[1].result = WORK_NG;
        else if (!_camResultStates.contains(3))
            _CameraEachpushArray[2].result = WORK_NG;
        else if (!_camResultStates.contains(4))
            _CameraEachpushArray[5].result = WORK_NG;
        else if (!_camResultStates.contains(5))
            _CameraEachpushArray[6].result = WORK_NG;

        _camResultStates.clear();

        // 汇总
        QString timeCur = timeStart.toString("yyyy-MM-dd  hh:mm:ss.zzz");
        this->onePush(timeCur);

        QDateTime timeEnd = QDateTime::currentDateTime();
        this->mainThread_log_received(
                    "\n [Summary  No] start: " + timeCur + "   end: " + timeEnd.toString("yyyy-MM-dd  hh:mm:ss.zzz") +
                    "   time： "+ QString::number(timeStart.msecsTo(timeEnd)) + " ms\n"
        );


    }

}


void VideoPanel::timer_timeout_calcSysteInfo()
{
    if ((_camResultStates.contains(2) && _camResultStates.contains(3) && _camResultStates.contains(4)) || _numDaoWei == 0){
        QtConcurrent::run([=]{
            int cpu = 0;
            int mem = 0;
            int disk = 0;
            int gpu = 0;

            cpu = getCPU();
            mem = getMemory();
            disk = getDisk();
            gpu = getGPU();

            emit emit_systemInfo(cpu, mem, disk, gpu);
        });
    }
}
void VideoPanel::get_isShowSysteminfo(bool isShow)
{
    if (isShow){
        this->timer_timeout_calcSysteInfo();

        _timer_calcSysteInfo->start(2000);
    }
    else{
        _timer_calcSysteInfo->stop();
    }
}


///////////// receive PLC state :
void VideoPanel::modbusTcpThread_PLCstate_received(int value)
{
    if (value == 1){  // 心跳
        QtConcurrent::run([=]{
//            MV_CC_DEVICE_INFO_LIST  m_stDevList;
//            memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
//            // ch:枚举子网内所有设备 | en:Enumerate all devices within subnet
//            MV_CC_EnumDevices(MV_GIGE_DEVICE, &m_stDevList);

//            if (m_stDevList.nDeviceNum != CameraRealNum){
            if (vcamdevice[3]->m_imvCamera->displayGigeDeviceInfo() < CameraRealNum){
                _isCameraError = true;

                _heartbeatCnts++;
                if (_heartbeatCnts >= 2){
                    _heartbeatCnts = 0;
                    emit emit_writeResultOrOffline(0x04, 2);
                    emit emit_cameraOffline(true);
                }
            }
            else{
                if (_isCameraError){
                    emit emit_cameraOffline(false);

                    this->updateCameraState();
                    _isCameraError = false;
                }
            }
        });
    }

    else if (value == 2){
        QDateTime time_daoWei = QDateTime::currentDateTime();
        if (!_timeDaoWei_last.isNull()){
            if (_timeDaoWei_last.msecsTo(time_daoWei) < 1200){

                this->mainThread_log_received(QString("\n>>>>>> [错误: 多触发] 到位信号: %1  时间戳：").arg(_numDaoWei)
                                              + time_daoWei.toString("yyyy-MM-dd  hh:mm:ss.zzz") + "\n");
                return;
            }
        }
        _timeDaoWei_last = time_daoWei;
        _timer_calcSysteInfo->stop();

        // 每个相机设置pushNum
        _numDaoWei++;
        switch (_numDaoWei)
        {
        case 1:
            break;
        case 2:
            _CamPushNums[0]++;
            break;
        case 3:
            _CamPushNums[0]++;
            _CamPushNums[1]++;
            break;
        case 4:
        case 5:
        case 6:
            _CamPushNums[0]++;
            _CamPushNums[1]++;
            _CamPushNums[2]++;
            break;
        case 7:
            _CamPushNums[0]++;
            _CamPushNums[1]++;
            _CamPushNums[2]++;
            _CamPushNums[3]++;
            break;
        default:  // 包含7
            _CamPushNums[0]++;
            _CamPushNums[1]++;
            _CamPushNums[2]++;
            _CamPushNums[3]++;
            _CamPushNums[4]++;
            break;

        }

        QString timeCur = time_daoWei.toString("yyyy-MM-dd  hh:mm:ss.zzz");
        this->mainThread_log_received(QString("\n >>>>>> 到位信号: %1  时间戳：%2\n").arg(_numDaoWei).arg(timeCur));

        if (_numDaoWei > 1){
            emit emit_cam1_pushNum(_CamPushNums[0], 0, time_daoWei);
            emit emit_cam2_pushNum(_CamPushNums[1], 0, time_daoWei);
            emit emit_cam3_pushNum(_CamPushNums[2], 0, time_daoWei);
            emit emit_cam4_pushNum(_CamPushNums[3], 0, time_daoWei);
            emit emit_cam5_pushNum(_CamPushNums[4], 0, time_daoWei);

            if (_numDaoWei > 2){
                this->initClear_and_onePush();

                if (_numDaoWei > CameraNum + 1){
                    _isPushInit = false;
                }
            }
        }

        if (_isPushInit){
            emit emit_writeResultOrOffline(0x03, 3);
        }

//        if (_numDaoWei % 2 == 1){
//            emit emit_writeResultOrOffline(0x03, 1);
//            emit emit_sendui(_numOK, _numNG, _numTotal, _numNGTypes);
//        }
//        else{
//            emit emit_writeResultOrOffline(0x03, 2);
//            emit emit_sendui(_numOK, _numNG, _numTotal, _numNGTypes);
//        }
        printf("%s\n", this->getCallBackString().toLatin1().data());
    }

    else if(value == 6) {  // 只清空OK NG数量,不清空状态
//        _numOK = 0;
//        _numNG = 0;
//        _numTotal = 0;
//        for (int i=0; i<_numNGTypes.count(); i++)
//        {
//            _numNGTypes[i] = 0;
//        }
//        emit emit_sendui(0, 0, 0, _numNGTypes);

//        query.exec(QString("select * from workpieces order by id desc limit 1 "));
//        while(query.next()) {
//            _numTotalAll = query.value(16).toInt();
//        }
    }
    else {
        _isPLCError = false;
    }
}


QString VideoPanel::getCallBackString()
{
    QString info = "\n############   trigger counts:  ";
    int sums = 0;
    for(int cnts: _camCallbackCnts)
    {
        sums += cnts;
        info += QString("%1   ").arg(cnts);
    }
    info += "total=" + QString::number(sums) + "  " + QDateTime::currentDateTime().toString("yyyy-MM-dd  hh:mm:ss.zzz");
    info += "\n";

    return info;
}

// camera trigger cnts
void VideoPanel::modbusTcpThread_callbackCamid_received(int camid)
{
    _camCallbackCnts[camid]++;

//    printf("%s\n", this->getCallBackString().toLatin1().data());
}




#ifdef TEST
void VideoPanel::full()
{
    if (actionFull->text() == "切换全屏模式") {
        emit fullScreen(true);
        actionFull->setText("切换正常模式");
    } else {
        emit fullScreen(false);
        actionFull->setText("切换全屏模式");
    }

    //执行全屏处理
}


////
void VideoPanel::rightMenu_software()
{
    _isStart_rightMenu = !_isStart_rightMenu;
    if (_isStart_rightMenu){
        _timer->start(3000);
    }
    else{
        _timer->stop();
    }
}


//// 1. local image

std::vector<std::string> _paths_duanMian = std::vector<std::string>();
std::vector<std::string> _paths_neiDaoJiao = std::vector<std::string>();
std::vector<std::string> _paths_waiDaoJiao = std::vector<std::string>();
std::vector<std::string> _paths_waiHuan = std::vector<std::string>();
int imgIndex = 0;
void VideoPanel::test_imageDir()
{
    QString dir_duanMian = _cpp2Dir + "/images/1";
    _paths_duanMian.clear();
    _paths_duanMian = GetFilesFromDir(dir_duanMian.toStdString(), "jpg", true);

    QString dir_waiDaoJiao = _cpp2Dir + "/images/2";
    _paths_waiDaoJiao.clear();
    _paths_waiDaoJiao = GetFilesFromDir(dir_waiDaoJiao.toStdString(), "jpg", true);

    QString dir_neiHuan = _cpp2Dir + "/images/3";
    _paths_neiDaoJiao.clear();
    _paths_neiDaoJiao = GetFilesFromDir(dir_neiHuan.toStdString(), "jpg", true);


    QString dir_waiHuan = _cpp2Dir + "/images/7";
    _paths_waiHuan.clear();
    _paths_waiHuan = GetFilesFromDir(dir_waiHuan.toStdString(), "jpg", true);


    for (int i = 0; i< 7; i++){
        test_getImage(QString::fromStdString(_paths_duanMian[i]), 0, 1, i+1, "");
         QThread::msleep(20);
    }
    for (int i = 0; i< 11; i++){
        test_getImage(QString::fromStdString(_paths_waiDaoJiao[i]), 0, 2, i+1, "");

        test_getImage(QString::fromStdString(_paths_neiDaoJiao[i]), 0, 3, i+1, "");
        QThread::msleep(20);
    }
    for(int i= 0; i< 25;i++){

        test_getImage(QString::fromStdString(_paths_waiHuan[i]), 0, 7, i+1, "");
        QThread::msleep(20);

    }
}

void VideoPanel::test_timeout()
{
    QString path_duanMian = QString(_paths_duanMian[imgIndex % 7].c_str());
    QString path_neiDaoJiaon = QString(_paths_neiDaoJiao[imgIndex % 7].c_str());
    QString path_waiDaoJiao = QString(_paths_waiDaoJiao[imgIndex % 7].c_str());
    QString path_waiHuan = QString(_paths_waiHuan[imgIndex % 7].c_str());
    printf("------------------------------------------\n");
    printf("端面path: %s \n", path_duanMian.toLatin1().data());
    printf("内环path: %s \n", path_neiDaoJiaon.toLatin1().data());
    printf("外环倒角path: %s \n", path_waiDaoJiao.toLatin1().data());
    printf("外环path: %s \n", path_waiHuan.toLatin1().data());

    int pushNum = 0;
    test_getImage(path_duanMian, pushNum, 0, imgIndex, "");
    test_getImage(path_neiDaoJiaon, pushNum, 1,imgIndex,  "");
//    test_getImage(path_waiDaoJiao, pushNum, 2, "_currentTime");
//    test_getImage(path_waiHuan, pushNum, 6, "_currentTime");

    imgIndex++;
    if (imgIndex % 7 == 0){
        imgIndex = 0;
        _timer->stop();
    }
}

void VideoPanel::test_getImage(QString path, int push_num, int camid, int campicid, QString timeCur)
{
    const auto t_start = std::chrono::high_resolution_clock::now();
    //把采集到的图发给服务器
    cv::Mat image = cv::imread(path.toStdString(), 1);
    const auto t_end1 = std::chrono::high_resolution_clock::now();

    ImageInfo pInfo;
    pInfo.pushNum = push_num;
    pInfo.camid=camid;
    pInfo.campicid= campicid;
    pInfo.width = image.cols;
    pInfo.height= image.rows;
    pInfo.channels = image.channels();
    pInfo.src_path = path;
    pInfo.dst_path = _settingJsons["save_cam1_dir"] + "/" + _saveFormats[_saveFormatIdx] + _saveSuffix;//path.replace(".bmp", "_res.bmp");
//    printf("[%s] pcam->src_path=%s\n",__func__, pInfo.src_path.toLatin1().data());
//    printf("[%s] pcam->campicid=%d\n",__func__, pInfo.campicid);

    const auto t_end7 = std::chrono::high_resolution_clock::now();
    pInfo.srcImage = image.clone();

    const auto t_end8 = std::chrono::high_resolution_clock::now();
    if (camid == 1){
        if (campicid < 4)
            vcamdevice[pInfo.camid-1]->m_mainThread->transform_data(pInfo);
        else
            vcamdevice[3]->m_mainThread->transform_data(pInfo);
    }
    else if (camid == 2 || camid == 3){
        vcamdevice[pInfo.camid-1]->m_mainThread->transform_data(pInfo);
    }
    else{
        vcamdevice[pInfo.camid+1]->m_mainThread->transform_data(pInfo);
    }

//    const auto t_end9 = std::chrono::high_resolution_clock::now();

//    printf("###############\n", __func__, pInfo.camid);

//    const auto t_end10 = std::chrono::high_resolution_clock::now();
//    const float ms1 = std::chrono::duration<float, std::milli>(t_end1 - t_start).count();
//    const float ms7 = std::chrono::duration<float, std::milli>(t_end7 - t_start).count();
//    const float ms8 = std::chrono::duration<float, std::milli>(t_end8 - t_start).count();
////    const float ms9 = std::chrono::duration<float, std::milli>(t_end9 - t_start).count();
//    const float ms10 = std::chrono::duration<float, std::milli>(t_end10 - t_start).count();
//    std::cout << "times________________________: "<< ms1  << ",  " << ms7  << ",  " << ms8 << ",  " << ms10 << std::endl;
}


//// 2. software trigger

void VideoPanel::test_timeout_daoWei()
{
    this->modbusTcpThread_PLCstate_received(2);

    QtConcurrent::run(this, &VideoPanel::test_timeout_software_7);
    QtConcurrent::run(this, &VideoPanel::test_timeout_software_2);
    QtConcurrent::run(this, &VideoPanel::test_timeout_software_3);
    QtConcurrent::run(this, &VideoPanel::test_timeout_software_5);
    QtConcurrent::run(this, &VideoPanel::test_timeout_software_6);
    QtConcurrent::run(this, &VideoPanel::test_timeout_software_1);
    QtConcurrent::run(this, &VideoPanel::test_timeout_software_4);
}

void VideoPanel::test_timeout_software_1()
{
    for(int i=0; i<7; i++)
    {
        vcamdevice[0]->m_mvsCamera->CommandExecute("TriggerSoftware");
        QThread::msleep(30);
    }
}
void VideoPanel::test_timeout_software_2()
{
    for(int i=0; i<11; i++)
    {
        vcamdevice[1]->m_mvsCamera->CommandExecute("TriggerSoftware");
        QThread::msleep(35);
    }
}
void VideoPanel::test_timeout_software_3()
{
    for(int i=0; i<11; i++)
    {
        vcamdevice[2]->m_mvsCamera->CommandExecute("TriggerSoftware");
        QThread::msleep(30);
    }
}
void VideoPanel::test_timeout_software_4()
{
    for(int i=0; i<7; i++)
    {
        vcamdevice[4]->m_mvsCamera->CommandExecute("TriggerSoftware");
        QThread::msleep(30);
    }
}
void VideoPanel::test_timeout_software_5()
{
    for(int i=0; i<11; i++)
    {
        vcamdevice[5]->m_mvsCamera->CommandExecute("TriggerSoftware");
        QThread::msleep(30);
    }
}
void VideoPanel::test_timeout_software_6()
{
    for(int i=0; i<11; i++)
    {
        vcamdevice[6]->m_mvsCamera->CommandExecute("TriggerSoftware");
        QThread::msleep(30);
    }
}
void VideoPanel::test_timeout_software_7()
{
    for(int i=0; i<25; i++)
    {
        vcamdevice[8]->m_mvsCamera->CommandExecute("TriggerSoftware");
        QThread::msleep(40);
    }
}

#endif

