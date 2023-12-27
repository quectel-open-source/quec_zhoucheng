/************************************************************************/
/* 以C++接口为基础，对常用函数进行二次封装，方便用户使用                */
/************************************************************************/

#ifndef _GX_CAMERA_H_
#define _GX_CAMERA_H_

#include "DaHengCamera/GxIAPI.h"
#include "DaHengCamera/DxImageProc.h"
#include "commonZack.h"
#include <string.h>
#include <QObject>
#include <opencv2/opencv.hpp>
#include <MVSCamera.h>

typedef struct CallBackInfo
{
    int camid;
    int campicid;
    int pushNum;
    cv::Mat img;
    int w;
    int h;
    QDateTime time_start;
}CallBackInfo;


static void GetErrorString(GX_STATUS emErrorStatus)
{
    char *                  error_info = NULL;
    size_t                  size = 0;

    GX_STATUS               emStatus = GX_STATUS_SUCCESS;

    // Get length of error description
    emStatus                        = GXGetLastError(&emErrorStatus, NULL, &size);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        printf("<Error when calling GXGetLastError>\n");
        return;
    }

    // Alloc error resources
    error_info      = new char[size];

    if (error_info == NULL)
    {
        printf("<Failed to allocate memory>\n");
        return;
    }

    // Get error description
    emStatus = GXGetLastError(&emErrorStatus, error_info, &size);

    if (emStatus != GX_STATUS_SUCCESS)
    {
        printf("<Error when calling GXGetLastError>\n");
    }
    else
    {
        printf("%s\n", (char *) error_info);
    }

    // Realease error resources
    if (error_info != NULL)
    {
        delete[] error_info;
        error_info      = NULL;
    }
}
//Show error message, close device and lib
#define GX_VERIFY_EXIT(emStatus) \
    if (emStatus != GX_STATUS_SUCCESS)     \
{                                      \
    GetErrorString(emStatus);          \
    GXCloseLib();                      \
    printf("<App Exit!>\n");           }
   // return emStatus;



class GXCamera : public QObject
{
    Q_OBJECT

public:
    GXCamera(QWidget* parent=0);
    ~GXCamera();

    void static gxImageCallBack(GX_FRAME_CALLBACK_PARAM *pFrameData);

    static void GX_STDC OnDeviceOfflineCallbackFun(void* pUserParam);

    // 初始化
    int initCamera(int camid, int ipid=1, GX_OPEN_MODE_CMD openMode=GX_OPEN_IP);

    int judgeCamid();

    void set_pushNum(int pushNum, int campicid, QDateTime timeCurrent);
    void set_saveDir(QString dir, QString mergeDir);

    unsigned short BLEndianUshort(unsigned short value);



    // ch:枚举设备 | en:Enumerate Device
    static int EnumDevices(uint32_t* punNumDevices, uint32_t nTimeOut);

    // ch:打开设备 | en:Open Device
    int Open(GX_OPEN_PARAM* pOpenParam, GX_DEV_HANDLE* phDevice);

    // ch:关闭设备 | en:Close Device
    int Close();

    // ch:注册图像数据回调 | en:Register Image Data CallBack
    int RegisterImageCallBack(GX_DEV_HANDLE hDevice, void *pUserParam, GXCaptureCallBack callBackFun);

    // ch:开启抓图 | en:Start Grabbing
    int StartGrabbing();

    // ch:停止抓图 | en:Stop Grabbing
    int StopGrabbing();

    int  CommandExecute(GX_FEATURE_ID_CMD featureID);

private slots:
    void ImageCallBackInner(int camid, int campicid, int pushNum, const void* imgBuf, int w, int h, QDateTime time_start);

signals:
    void emit_sendFrame(int camid, int campicid, int pushNum, const void* imgBuf, int w, int h, QDateTime time_start);
    void emit_imageInfo(ImageInfo pInfo); // , unsigned char* buf);
    void emit_imageInfo_others(ImageInfo pInfo);
    void emit_log(QString info);

    void emit_offline(int camid);
    void emit_callbackCamid(int camid);

public:
    GX_DEV_HANDLE g_hDevice;
    int64_t g_i64ColorFilter;
    GX_STATUS       emStatus;

    int _camid;
    int _campicid;
    int _pic_cnts;
    int _pushNum;
    QString _saveDir;
    QString _mergeDir;

    QDateTime _time_daoWei;
    QDateTime _timeFirst;

    QQueue<CallBackInfo> _callbacks;
    bool _isEnd;

};

#endif//_MV_CAMERA_H_
