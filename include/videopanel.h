#ifndef VIDEOPANEL_H
#define VIDEOPANEL_H

/**
 * 视频监控画面控件 整理:feiyangqingyun(QQ:517216493) 2019-04-11
 * 1. 可设定视频通道数量。
 * 2. 支持双击最大化再次双击还原。
 * 3. 支持4/6/8/9/13/16/25/36/64等通道布局。
 * 4. 内置了选中边框高亮等样式。
 * 5. 通用的视频通道布局盒子类，方便拓展其他布局。
 */

#include <QWidget>
#include "GXCamera.h"
#include "MVSCamera.h"
#include "DaHuaCamera.h"
#include "ModbusTcpThread.h"
#include "mainThread.h"
#include "commonZack.h"
#include <QMap>
#include <QMainWindow>
#include <QMenu>

//#define USING_MODBUS


//#define TEST

class VideoPanel : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPanel(QWidget *parent = 0);
    ~VideoPanel();

    int onePush(QString timeCur);
    void loadHistoryResult();
    void updateCameraState();
    void update_chars(QString chars1, QString chars2);

    void init_nums();

    int  checktotall;
    int  checkng;
    int  checkok;
    int  num;

    int _numCallback;
    int _timeTotal;
    int _initCnts;
    float _initTimes;
    int _heartbeatCnts;

    QTimer* _timer;
//    QList<QTimer*> _timers_software;
    bool _isStart_rightMenu;
    QTimer* _timer_calcSysteInfo;
    bool _is_getAllResults;


#ifdef USING_MODBUS
    // Modbus
    ModbusTcpThread* _modbusTcpThread;
#else
    // Fins
    OmronFinsTcpThread* _modbusTcpThread;
#endif

    QList<QVariant> _camStates;
    QList<QString> _charsInit;

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    int videoCount;             //视频通道个数
    QMenu *videoMenu;           //右键菜单
    QAction *actionFull;        //全屏动作
    QAction *actionPoll;        //轮询动作
    QAction *actionTest;

    QGridLayout* _layout;
    bool _isLabelMax;
    QList<QLabel*> _widgets;        //视频控件集合

    std::vector<std::vector<int>> _labelsTotal;
    QDateTime _timeDaoWei_last;

public:
    void start_closed();
    void show_allLabels();
    void hide_allLabels();

    int vtcpServerThreadCreate(void);
    int     vtcpServereDestroy(void);
    int     vfd;

    void JudgeCameraState(int cnts);

    void timer_timeout_calcSysteInfo();
    void get_isShowSysteminfo(bool isShow);

private slots:

    void initForm();
    void initForm_setSaveDir();

    void initForm_MVSCameras();
    void initForm_GxCamera();

    void initMenu();

    void rightMenu_startSoftware();

#ifdef TEST
    void full();
    void rightMenu_software();

    void test_timeout();
    void test_imageDir();
    void test_getImage(QString path, int push_num, int camid, int campicid, QString timeCur);

    void test_timeout_daoWei();
    void test_timeout_software_1();
    void test_timeout_software_2();
    void test_timeout_software_3();
    void test_timeout_software_4();
    void test_timeout_software_5();
    void test_timeout_software_6();
    void test_timeout_software_7();
#endif

    void set_onePush(int camid);

    void mainThread_log_received(QString text);
    void mainThread_initState(int initState, float loaded_time);
    void mainThread_results_1_received(int camid, int campicid, QImage dstImage, int result, QString SN, float area_one);
    void mainThread_results_2_received(int camid, int campicid, QImage dstImage, int result, QString SN, float area_one);
    void mainThread_results_3_received(int camid, int campicid, QImage dstImage, int result, QString SN, float area_one);
    void mainThread_results_4_received(int camid, int campicid, QImage dstImage, int result, QString SN, float area_one);
    void mainThread_results_5_received(int camid, int campicid, QImage dstImage, int result, QString SN, float area_one);

    void mainThread_initChar_received(int camid, QList<QString> keys);

public slots:
    void update_preParams(int camid, QList<int> pre_params);
    void update_preROI(int camid, QList<int> pre_ROI);
    void update_postParams(int camid, QList<QString> label_list, QMap<QString, QList<int>> labels_params);

    void modbusTcpThread_PLCstate_received(int value);
    void modbusTcpThread_callbackCamid_received(int camid);
    QString getCallBackString();

    void initClear_and_onePush();

signals:

    //全屏切换信号
    void modbusbeat();
    void emit_writeResultOrOffline(int address, int value);
    void emit_cameraOffline(bool isOffline);

    void fullScreen(bool v);
    void senduitotall(int value);
    void senduing(int value);
    void senduiok(int value);
    void emit_sendui(int ok, int ng, int total, QList<int> numNGTypes);

    void sendCallbackImg(QImage image, int label_id);
    void emit_jsonDir(QMap<QString, QString> jsonMap);
    void emit_inferState(int initState);
    void emit_updateCamera(int net, QString ipStr, int state);

    void emit_init_photo_store();
    void emit_onePush(int camid);

    void emit_startCalcSystemInfo(int camid);
    void emit_systemInfo(int cpu, int mem, int disk, int gpu);

    void emit_cam1_pushNum(int pushNum, int campicid, QDateTime timeCurrent);  // MVSCamera
    void emit_cam2_pushNum(int pushNum, int campicid, QDateTime timeCurrent);  // MVSCamera
    void emit_cam3_pushNum(int pushNum, int campicid, QDateTime timeCurrent);  // MVSCamera
    void emit_cam4_pushNum(int pushNum, int campicid, QDateTime timeCurrent);  // MVSCamera
    void emit_cam5_pushNum(int pushNum, int campicid, QDateTime timeCurrent);  // MVSCamera
    void emit_cam6_pushNum(int pushNum, int campicid, QDateTime timeCurrent);  // MVSCamera
    void emit_cam7_pushNum(int pushNum, int campicid, QDateTime timeCurrent);  // MVSCamera

    void emit_initChar(QList<QString> keys);
};


//#define USING_MVS_CAMERA
struct CamDevice
{
    int  id;
    QLabel* mlabel;
//    void *m_hWnd;

//    MVSCamera*         m_mvsCamera;
    GXCamera*          m_gxCamera=NULL;
    DaHuaCamera*         m_imvCamera=NULL;
//    bool               m_bGrabbing;
    VideoPanel*        m_v;
    MainThread*        m_mainThread;
};


#endif // VIDEOPANEL_H
