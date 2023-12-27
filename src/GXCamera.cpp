#include "GXCamera.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <sys/socket.h>
#include "qsqlquery.h"
#include <QtConcurrent>
#include <QMetaType>

using namespace std;

#define ACQ_BUFFER_NUM  5

static sem_t semSend;
extern QString _cpp2Dir;


GXCamera::GXCamera(QWidget* parent):QObject(parent)
{
    g_hDevice= NULL;
    emStatus = GX_STATUS_SUCCESS;

    _pushNum = 0;
    _camid = -2;
    _campicid = 0;
    _isEnd = true;

    qRegisterMetaType<CallBackInfo>("CallBackInfo");
    connect(this, &GXCamera::emit_sendFrame, this, &GXCamera::ImageCallBackInner);
}

GXCamera::~GXCamera()
{
   // if (m_hDevHandle)
    {
 //       MV_CC_DestroyHandle(m_hDevHandle);
 //       m_hDevHandle    = MV_NULL;
    }

}


int GXCamera::judgeCamid()
{
    printf("  camera.camid=%d\n", _camid);
    return _camid;
}

/////////
void GXCamera::set_pushNum(int pushNum, int campicid, QDateTime timeCurrent)
{
    _pushNum = pushNum;
//    if (_campicid == 1)
//        _campicid = 1;
//    else
        _campicid = campicid;

    _time_daoWei = timeCurrent;
//    _isNewPush = true;
}

void GXCamera::set_saveDir(QString dir, QString mergeDir)
{
    _saveDir = dir;
    _mergeDir = mergeDir;
}


//// 掉线回调处理函数
void GX_STDC GXCamera::OnDeviceOfflineCallbackFun(void* pUserParam)
{
    GXCamera *pcam = (GXCamera*)pUserParam;
    //收到掉线通知后,由用户主动发送通知主线程停止采集或者关闭设备等操作 ...
    printf("---->[except] camera except!!\n");



//    char *buf = NULL;
//    int sendlen = sizeof(MsgCenterHead) + sizeof(Msg_exceptOut) + 10;
//    buf = (char *)malloc(sendlen);
//    memcpy(buf, &mhead, sizeof (MsgCenterHead));

//    memcpy(buf+sizeof(MsgCenterHead), &msgOut, sizeof(Msg_exceptOut));
//    memcpy(buf+sizeof(MsgCenterHead)+sizeof(Msg_exceptOut), msg.c_str(), 10);

}

////
void GXCamera::gxImageCallBack(GX_FRAME_CALLBACK_PARAM *pFrameData)
{
    GXCamera *pcam = (GXCamera*)pFrameData->pUserParam;
    QDateTime time_start = QDateTime::currentDateTime();

    pcam->_campicid++;
    if (pcam->_campicid == 1){
        pcam->_timeFirst = time_start;
    }

    if (pcam->_campicid > pcam->_pic_cnts){
        pcam->_campicid = 1;
        pcam->_pushNum += 1;

    }
    if (pFrameData->status == GX_FRAME_STATUS_INCOMPLETE){
        emit pcam->emit_log(QString(">>>> wrong: frame status incomplete,  %1__%2__%3\n").arg(pcam->_pushNum).arg(pcam->_camid).arg(pcam->_campicid));
    }

    emit pcam->emit_sendFrame(pcam->_camid, pcam->_campicid, pcam->_pushNum, pFrameData->pImgBuf, pFrameData->nWidth, pFrameData->nHeight, time_start);
    emit pcam->emit_callbackCamid(pcam->_camid-1);
    printf("#### callback DaHeng:  %d  %d__%d,  %dx%d,  %s\n", pcam->_pushNum, pcam->_camid, pcam->_campicid, pFrameData->nWidth, pFrameData->nHeight, time_start.toString("yyyy-MM-dd hh:mm:ss.zzz").toLatin1().data());

}

void GXCamera::ImageCallBackInner(int camid, int campicid, int pushNum, const void* imgBuf, int w, int h, QDateTime time_start)
{
//    printf("  camid=%d, campicid=%d, start store !!!   %d x %d   %s\n", camid, campicid, w, h, QTime::currentTime().toString("hh:mm:ss.zzz").toLatin1().data());
//    CallBackInfo callbackInfo;
//    callbackInfo.camid = camid;
//    callbackInfo.campicid = campicid;
//    callbackInfo.pushNum = pushNum;
//    callbackInfo.w = w;
//    callbackInfo.h = h;
//    callbackInfo.time_start = time_start;

//    unsigned char* rgb24 = (unsigned char*)malloc(callbackInfo.w * callbackInfo.h);
//    memcpy(rgb24, (unsigned char*)imgBuf, callbackInfo.w * callbackInfo.h);
//    cv::Mat srcImage = cv::Mat(callbackInfo.h, callbackInfo.w, CV_8UC1, rgb24);
//    callbackInfo.img = srcImage.clone();
//    _callbacks.append(callbackInfo);

//    free(rgb24);

//    if (_isEnd){
//        while (!_callbacks.isEmpty())
//        {
//            _isEnd = false;

//            int time_0 = callbackInfo.time_start.msecsTo(QDateTime::currentDateTime());
//            CallBackInfo info = _callbacks.dequeue();

//            ImageInfo pInfo;
//            pInfo.camid = info.camid;
//            pInfo.campicid = info.campicid;
//            pInfo.pushNum = info.pushNum;
//            pInfo.width = info.w;
//            pInfo.height = info.h;
//            pInfo.time_start = _timeFirst;

//            pInfo.SN = Int2QString(pInfo.pushNum);

//            pInfo.prefix = _saveFormats[_saveFormatIdx].arg(pInfo.SN).arg(_timeFirst.toString("hh-mm-ss")).arg(pInfo.campicid);
//            pInfo.src_path = QString("%1/%2_0_org.").arg(_saveDir).arg(pInfo.prefix) + _saveSuffix;
//            pInfo.dst_path = QString("%1/%2_0_org.").arg(_saveDir).arg(pInfo.prefix) + _saveSuffix;
//            pInfo.merge_path = QString("%1/%2_%3_0_org.").arg(_mergeDir).arg(pInfo.SN).arg(_timeFirst.toString("hh-mm-ss")) + _saveSuffix;

//            if(info.w < 10 || info.h < 10){
//                emit emit_log(QString(">>>> wrong: loss image,  stDisplayInfo.nWidth=%1, stDisplayInfo.height=%2\n").arg(info.w).arg(info.h));
//                return;
//            }

////            int bodylength = info.w * info.h;
////            unsigned char* rgb24 = NULL;
////            rgb24 = (unsigned char*)malloc(bodylength);
////            memset(rgb24, 0, bodylength);

////            memcpy(rgb24, (unsigned char*)info.imgBuf, bodylength);
//    //        DxRaw8toRGB24((unsigned char*)imgBuf, rgb24, w, h, RAW2RGB_NEIGHBOUR, DX_PIXEL_COLOR_FILTER(g_i64ColorFilter), false);
//            printf("  camid=%d, campicid=%d, DxRaw8toRGB24 completed !!!   %d x %d\n", info.camid, info.campicid, info.w, info.h);
//            int time_1 = info.time_start.msecsTo(QDateTime::currentDateTime());

//            pInfo.srcImage = info.img.clone();

//            emit emit_imageInfo(pInfo);
//            int time_2 = info.time_start.msecsTo(QDateTime::currentDateTime());

//            if (_saveImgsNums == 0)
//                cv::imwrite(pInfo.src_path.toStdString(), callbackInfo.img);

//            int time_total = info.time_start.msecsTo(QDateTime::currentDateTime());
////            printf("  ## %d  %d_%d  write_time=%d ms\n", callbackInfo.pushNum, pInfo.camid, pInfo.campicid, time_total);
//            emit emit_log(QString(">>>>  %1__%2__%3   %4   saveImg: %5/%6/%7/%8 ms")
//                                   .arg(pInfo.SN).arg(pInfo.camid).arg(pInfo.campicid)
//                                   .arg(info.time_start.toString("yyyy-MM-dd  hh-mm-ss.zzz"))
//                                   .arg(time_0).arg(time_1).arg(time_2).arg(time_total));

//            _isEnd = true;
//        }
//    }

    int time_0 = time_start.msecsTo(QDateTime::currentDateTime());

    ImageInfo pInfo;
    pInfo.camid = camid;
    pInfo.campicid = campicid;
    pInfo.pushNum = pushNum;
    pInfo.width = w;
    pInfo.height = h;
    pInfo.time_start = _timeFirst;

    pInfo.SN = Int2QString(pInfo.pushNum);

    pInfo.prefix = _saveFormats[_saveFormatIdx].arg(pInfo.SN).arg(_timeFirst.toString("hh-mm-ss")).arg(pInfo.campicid);
    pInfo.src_path = QString("%1/%2_0_org.").arg(_saveDir).arg(pInfo.prefix) + _saveSuffix;
    pInfo.dst_path = QString("%1/%2_0_org.").arg(_saveDir).arg(pInfo.prefix) + _saveSuffix;
    pInfo.merge_path = QString("%1/%2_%3_0_org.").arg(_mergeDir).arg(pInfo.SN).arg(_timeFirst.toString("hh-mm-ss")) + _saveSuffix;

    if(w < 10 || h < 10){
        emit emit_log(QString(">>>> wrong: loss image,  stDisplayInfo.nWidth=%1, stDisplayInfo.height=%2\n").arg(w).arg(h));
        return;
    }
//    unsigned char* rgb24 = (unsigned char*)malloc(w*h);
//    memcpy(rgb24, (unsigned char *)imgBuf, w*h);
    pInfo.srcData = (unsigned char*)malloc(w*h);
    memcpy(pInfo.srcData, (unsigned char *)imgBuf, w*h);

//    pInfo.data = std::shared_ptr<unsigned char>(rgb24, free);

    printf("  camid=%d, campicid=%d, DxRaw8toRGB24 completed !!!   %d x %d\n", camid, campicid, w, h);
    int time_1 = time_start.msecsTo(QDateTime::currentDateTime());

//    cv::Mat srcImage(h, w, CV_8UC1, rgb24.get());
//    pInfo.srcImage = srcImage.clone();

    if (pInfo.campicid == 1){
        emit emit_imageInfo(pInfo);
    }
    else{
        emit emit_imageInfo_others(pInfo);
    }
    int time_2 = time_start.msecsTo(QDateTime::currentDateTime());

    if (_saveImgsNums == 0){
        cv::Mat srcImage(h, w, CV_8UC1, pInfo.srcData);
        cv::imwrite(pInfo.src_path.toStdString(), srcImage);
    }

//    free(rgb24);

    int time_total = time_start.msecsTo(QDateTime::currentDateTime());
    printf("  ## %d  %d_%d  write_time=%d ms\n", pushNum, pInfo.camid, pInfo.campicid, time_total);
    emit emit_log(QString(">>>>  %1__%2__%3    %4  saveImg: %5/%6/%7/%8 ms")
                           .arg(pInfo.SN).arg(pInfo.camid).arg(pInfo.campicid)
                           .arg(time_start.toString("yyyy-MM-dd  hh-mm-ss.zzz"))
                           .arg(time_0).arg(time_1).arg(time_2).arg(time_total));
}



////////////////////
///    GX_OPEN_SN              =0,        ///< 通过SN打开
///    GX_OPEN_IP              =1,        ///< 通过IP打开
///    GX_OPEN_MAC             =2,        ///< 通过MAC打开
///    GX_OPEN_INDEX           =3,        ///< 通过Index打开
///    GX_OPEN_USERID          =4,        ///< 通过用户自定义ID打开
int GXCamera::initCamera(int camid, int ipid, GX_OPEN_MODE_CMD openMode)
{
    printf("---->>>> start init, camid=%d\n", camid);
    _camid = camid;
    switch (camid){
    case 1:
    case 5:
        _pic_cnts = 1;
        break;
    case 2:
    case 4:
        _pic_cnts = 5;
        break;
    case 3:
        _pic_cnts = 1;
        break;
    }

    GX_OPEN_PARAM  stOpenParam;
    memset(&stOpenParam,0,sizeof(stOpenParam));
    stOpenParam.accessMode =  GX_ACCESS_EXCLUSIVE;
    stOpenParam.openMode = openMode;//GX_OPEN_IP;

    char ip_str[128];
    memset(ip_str,0,128);

    switch (openMode) {
    case GX_OPEN_SN:
        break;
    case GX_OPEN_IP:
        sprintf(ip_str, "192.168.%d.100", ipid);
        printf("[%s] getCameraHandle num:%d , < ip=%s >, ",__func__, camid, ip_str);
        break;
    case GX_OPEN_INDEX:
        sprintf(ip_str, "%d", ipid);
        printf("[%s] getCameraHandle num:%d , < index=%s >, ",__func__, camid, ip_str);
        break;
    case GX_OPEN_MAC:
        break;
    case GX_OPEN_USERID:
        break;

    }

    stOpenParam.pszContent= ip_str;
    int nRet = -1;
    nRet = GXOpenDevice(&stOpenParam, &g_hDevice);
//    nRet= GXOpenDeviceByIndex(ipid, &g_hDevice);
    //memcpy(vcamdevice[i]->m_gxCamera->g_hDevice,gx_hDevice[i],sizeof (GX_DEV_HANDLE));

//    nRet = Open(&stOpenParam, &g_hDevice);
    if (nRet != GX_STATUS_SUCCESS){
        printf("open failed!\n");
        GetErrorString(emStatus);
        emStatus = GXCloseLib();
        return -1;
    }
    else{
        printf("open success!\n");
    }


    //Get string length of Serial number
    size_t  nSize = 0;
    emStatus = GXGetStringLength(g_hDevice, GX_STRING_DEVICE_SERIAL_NUMBER, &nSize);
    GX_VERIFY_EXIT(emStatus);

    //Alloc memory for Serial number
    char *  pszSerialNumber = new char[nSize];
    //Get Serial Number
    emStatus = GXGetString(g_hDevice, GX_STRING_DEVICE_SERIAL_NUMBER, pszSerialNumber, &nSize);
    if (emStatus != GX_STATUS_SUCCESS)
    {
        delete[] pszSerialNumber;
        pszSerialNumber         = NULL;
        GX_VERIFY_EXIT(emStatus);
        return -1;
    }

    printf("[%s] <Serial Number : %s>\n",__func__, pszSerialNumber);
    //Release memory for Serial number
    delete[] pszSerialNumber;
    pszSerialNumber         = NULL;

    bool g_bColorFilter = false;
    int64_t g_nPayloadSize = 0;
    emStatus = GXIsImplemented(g_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &g_bColorFilter);
    GX_VERIFY_EXIT(emStatus);

    emStatus = GXGetInt(g_hDevice, GX_INT_PAYLOAD_SIZE, &g_nPayloadSize);
    GX_VERIFY_EXIT(emStatus);


    //Set acquisition mode
    emStatus                        = GXSetEnum(g_hDevice, GX_ENUM_ACQUISITION_MODE, GX_ACQ_MODE_CONTINUOUS);
    GX_VERIFY_EXIT(emStatus);

    //Set trigger mode
    emStatus                        = GXSetEnum(g_hDevice, GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_ON);
    GX_VERIFY_EXIT(emStatus);

    if (_saveTriggerMode.toUpper() == "SOFTWARE"){
//        nRet = this->CommandExecute(GX_COMMAND_TRIGGER_SOFTWARE);
        emStatus = GXSendCommand(g_hDevice, GX_COMMAND_TRIGGER_SOFTWARE);
//        nRet =  GXSetEnum(g_hDevice, GX_ENUM_TRIGGER_SOURCE, GX_COMMAND_TRIGGER_SOFTWARE);
    }
    else{
//        nRet = this->CommandExecute(GX_TRIGGER_SOURCE_LINE2); // GX_TRIGGER_SOURCE_ENTRY
        emStatus =  GXSetEnum(g_hDevice, GX_ENUM_TRIGGER_SOURCE, GX_TRIGGER_SOURCE_LINE2);
    }
    GX_VERIFY_EXIT(emStatus);
    if(emStatus == GX_STATUS_SUCCESS){
        printf("[%s] trigger mode: %s\n", __func__, _saveTriggerMode.toUpper().toLatin1().data());
    }
    else{
        printf("[%s] trigger set failed! ErrorCode[%d]\n", __func__, nRet);
        return -1;
    }

    emStatus                        = GXSetEnum(g_hDevice,GX_ENUM_TRIGGER_ACTIVATION, GX_TRIGGER_ACTIVATION_FALLINGEDGE);
    if(emStatus == GX_STATUS_SUCCESS){
        printf("[%s] triggerActivation Success:  FallingEdge\n", __func__);
    }

    //设置相机的流通道包长属性,提高网络相机的采集性能
    bool bImplementPacketSize = false;
    uint32_t unPacketSize = 1500; //0;
    // 判断设备是否支持流通道数据包功能
    emStatus = GXIsImplemented(g_hDevice, GX_INT_GEV_PACKETSIZE, &bImplementPacketSize);
    if (bImplementPacketSize)
    {
//            // 获取当前网络环境的最优包长
//            emStatus = GXGetOptimalPacketSize (gx_hDevice[i], &unPacketSize);
        // 将最优包长设置为当前设备的流通道包长值
        emStatus = GXSetInt (g_hDevice, GX_INT_GEV_PACKETSIZE, unPacketSize);
        GX_VERIFY_EXIT(emStatus);
        printf("[%s] id=%d, ret=%d  流通道包长值=%d\n", __FUNCTION__, camid, emStatus, unPacketSize);
    }
    else{
        printf("[%s] id=%d, ret=%d  不支持流通道包长\n", __FUNCTION__, camid, emStatus);
    }

//        // 设置相机触发延迟:us
//        emStatus = GXSetEnum(gx_hDevice[i], GX_ENUM_TRIGGER_SELECTOR, GX_ENUM_TRIGGER_SELECTOR_FRAME_START);
//        GX_VERIFY_EXIT(emStatus);
//        emStatus = GXSetFloat(gx_hDevice[i], GX_FLOAT_TRIGGER_DELAY, 2000000.0000);
//        GX_VERIFY_EXIT(emStatus);

//        // 设置传输数据块数量:32
//        uint64_t blockCnts = 32;
//        emStatus = GXSetInt(gx_hDevice[i], GX_DS_INT_STREAM_TRANSFER_NUMBER_URB, blockCnts);
//        printf("[%s] id=%d, ret=%d  设置传输数据块数量=%d\n", __func__, i, emStatus, blockCnts);
//        GX_VERIFY_EXIT(emStatus);

    //Set buffer quantity of acquisition queue
    uint64_t nBufferNum = ACQ_BUFFER_NUM;
    emStatus = GXSetAcqusitionBufferNumber(g_hDevice, nBufferNum);
    GX_VERIFY_EXIT(emStatus);

    bool bStreamTransferSize = false;
    emStatus = GXIsImplemented(g_hDevice, GX_DS_INT_STREAM_TRANSFER_SIZE, &bStreamTransferSize);
    GX_VERIFY_EXIT(emStatus);

    bool bStreamTransferNumberUrb = false;
    emStatus = GXIsImplemented(g_hDevice, GX_DS_INT_STREAM_TRANSFER_NUMBER_URB, &bStreamTransferNumberUrb);
    GX_VERIFY_EXIT(emStatus);
    // Set whitbalance


    //Get the type of Bayer conversion. whether is a color camera.
    emStatus = GXIsImplemented(g_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &g_bColorFilter);
    GX_VERIFY_EXIT(emStatus);
    if (g_bColorFilter)
    {
        emStatus = GXGetEnum(g_hDevice, GX_ENUM_PIXEL_COLOR_FILTER, &g_i64ColorFilter);
        GX_VERIFY_EXIT(emStatus);
    }else{
        g_i64ColorFilter = GX_COLOR_FILTER_NONE;
    }
    printf("[%s] id=%d g_i64ColorFilter=%ld\n",__func__, camid, g_i64ColorFilter);

    nRet = GXRegisterCaptureCallback(g_hDevice, this, gxImageCallBack);
    if (nRet != GX_STATUS_SUCCESS)
    {
        printf("[%s] GXRegisterCaptureCallback err,exit, ret=%d\n", __func__, nRet);
        GX_VERIFY_EXIT(emStatus);
        return - 1;
    }else{
        printf("[%s] id=%d GXRegisterCaptureCallback success, ret=%d\n", __func__, camid, nRet);
    }

//    //定义掉线回调函数句柄
//    GX_EVENT_CALLBACK_HANDLE hCB;
//    //注册设备掉线回调函数
//    GXRegisterDeviceOfflineCallback(g_hDevice, NULL, OnDeviceOfflineCallbackFun, &hCB);

    //注销设备掉线事件回调
//    GXUnregisterDeviceOfflineCallback(g_hDevice,hCB);

    nRet =   StartGrabbing();
    if (nRet != GX_STATUS_SUCCESS){
        //Release the memory allocated
        printf("[%s] GXStreamOn err,exit,i=%d\n", __func__, camid);
        GX_VERIFY_EXIT(emStatus);
        return -1;
    }
    else{
        printf("[%s] GXStreamOn success\n",__func__);
    }
    printf("--------------------------- end\n");

    return 0;

}





// ch:枚举设备 | en:Enumerate Device
int GXCamera::EnumDevices(uint32_t* punNumDevices, uint32_t nTimeOut)
{
    return GXUpdateAllDeviceList(punNumDevices, nTimeOut);
}


// ch:打开设备 | en:Open Device
int GXCamera::Open(GX_OPEN_PARAM* pOpenParam, GX_DEV_HANDLE* phDevice)
{
    int ret=GXOpenDevice(pOpenParam, phDevice);
   // g_hDevice = phDevice;
    memcpy(&g_hDevice,phDevice,sizeof (GX_DEV_HANDLE));
    return  ret;
}

// ch:关闭设备 | en:Close Device
int GXCamera::Close()
{

    return  GXCloseLib();
}


// ch:注册图像数据回调 | en:Register Image Data CallBack
int GXCamera::RegisterImageCallBack (GX_DEV_HANDLE hDevice, void *pUserParam, GXCaptureCallBack callBackFun)
{
   // return MV_CC_RegisterImageCallBackForBGR(m_hDevHandle, cbOutput, pUser);
     return GXRegisterCaptureCallback(hDevice,(void *)pUserParam,(GXCaptureCallBack)callBackFun);;
}

// ch:开启抓图 | en:Start Grabbing
int GXCamera::StartGrabbing()
{
    return GXStreamOn(g_hDevice);
}

// ch:停止抓图 | en:Stop Grabbing
int GXCamera::StopGrabbing()
{
    return GXStreamOff(g_hDevice);
}


int GXCamera::CommandExecute(GX_FEATURE_ID_CMD featureID)
{
    return GXSendCommand(g_hDevice, featureID);
}
