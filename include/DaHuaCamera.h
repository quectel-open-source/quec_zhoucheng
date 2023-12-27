#ifndef DAHUACAMERA_H
#define DAHUACAMERA_H

#include "DaHuaInclude/IMVApi.h"
#include "commonZack.h"
#include <string.h>
#include <QObject>
#include <opencv2/opencv.hpp>
#include "MVSCamera.h"
#include "DaHengCamera/GxIAPI.h"
#include "DaHengCamera/DxImageProc.h"


#ifndef IMV_NULL
#define IMV_NULL    0
#endif

static void displayDeviceInfo(IMV_DeviceList deviceInfoList)
{
    IMV_DeviceInfo* pDevInfo = NULL;
    unsigned int cameraIndex = 0;
    char vendorNameCat[11];
    char cameraNameCat[16];

    // 打印Title行
    // Print title line
    printf("\nIdx Type Vendor     Model      S/N             DeviceUserID    IP Address    \n");
    printf("------------------------------------------------------------------------------\n");

    for (cameraIndex = 0; cameraIndex < deviceInfoList.nDevNum; cameraIndex++)
    {
        pDevInfo = &deviceInfoList.pDevInfo[cameraIndex];
        // 设备列表的相机索引  最大表示字数：3
        // Camera index in device list, display in 3 characters
        printf("%-3d", cameraIndex + 1);

        // 相机的设备类型（GigE，U3V，CL，PCIe）
        // Camera type
        switch (pDevInfo->nCameraType)
        {
        case typeGigeCamera:printf(" GigE");break;
        case typeU3vCamera:printf(" U3V ");break;
        case typeCLCamera:printf(" CL  ");break;
        case typePCIeCamera:printf(" PCIe");break;
        default:printf("     ");break;
        }

        // 制造商信息  最大表示字数：10
        // Camera vendor name, display in 10 characters
        if (strlen(pDevInfo->vendorName) > 10)
        {
            memcpy(vendorNameCat, pDevInfo->vendorName, 7);
            vendorNameCat[7] = '\0';
            strcat(vendorNameCat, "...");
            printf(" %-10.10s", vendorNameCat);
        }
        else
        {
            printf(" %-10.10s", pDevInfo->vendorName);
        }

        // 相机的型号信息 最大表示字数：10
        // Camera model name, display in 10 characters
        printf(" %-10.10s", pDevInfo->modelName);

        // 相机的序列号 最大表示字数：15
        // Camera serial number, display in 15 characters
        printf(" %-15.15s", pDevInfo->serialNumber);

        // 自定义用户ID 最大表示字数：15
        // Camera user id, display in 15 characters
        if (strlen(pDevInfo->cameraName) > 15)
        {
            memcpy(cameraNameCat, pDevInfo->cameraName, 12);
            cameraNameCat[12] = '\0';
            strcat(cameraNameCat, "...");
            printf(" %-15.15s", cameraNameCat);
        }
        else
        {
            printf(" %-15.15s", pDevInfo->cameraName);
        }

        // GigE相机时获取IP地址
        // IP address of GigE camera
        if (pDevInfo->nCameraType == typeGigeCamera)
        {
            printf(" %s", pDevInfo->DeviceSpecificInfo.gigeDeviceInfo.ipAddress);
        }

        printf("\n");
    }

    return;
}


class DaHuaCamera : public QObject
{
    Q_OBJECT

public:
    explicit DaHuaCamera(QWidget* parent=0);
    ~DaHuaCamera();

    void set_pushNum(int pushNum, int campicid, QDateTime timeCurrent);
    void set_saveDir(QString dir, QString mergeDir);

    int initCamera(int camid, QString ip, int triggerMode=0);
    int displayGigeDeviceInfo(bool isShow=false);
private:
    static void dahuaImageCallBack(IMV_Frame* pFrame, void* pUser);
    // 初始化

    int _camid;
    int _campicid;
    int _pic_cnts;
    int _pushNum;
    QString _saveDir;
    QString _mergeDir;

    QDateTime _time_daoWei;
    QDateTime _timeFirst;

private slots:
    void ImageCallBackInner(int camid, int campicid, int pushNum, unsigned char* imgBuf, int w, int h, QDateTime time_start);


signals:
    void emit_sendFrame(int camid, int campicid, int pushNum, unsigned char* imgBuf, int w, int h, QDateTime time_start);
    void emit_log(QString info);
    void emit_imageInfo(ImageInfo pInfo); // , unsigned char* buf);

    void emit_callbackCamid(int camid);


public:
    // ch:枚举设备 | en:Enumerate Device
     int EnumDevices(IMV_DeviceList *pDeviceList, unsigned int interfaceType);

    // ch:打开设备 | en:Open Device
    int Open(char *specifiedStr,int softTrigger);

    // ch:关闭设备 | en:Close Device
    int Close();

    // ch:注册图像数据回调 | en:Register Image Data CallBack
    int RegisterImageCallBack(IMV_FrameCallBack proc, void* pUser);

    // ch:开启抓图 | en:Start Grabbing
    int StartGrabbing();

    // ch:停止抓图 | en:Stop Grabbing
    int StopGrabbing();
    int CommandExecute(const char* strKey);
    int CommandExcuteLine(const char* strKey);
private:
    void*               m_hDevHandle;


};
#endif // DAHUACAMERA_H
