#include "DaHuaCamera.h"
#include <QtConcurrent>
#include <QMetaType>


DaHuaCamera::DaHuaCamera(QWidget* parent):QObject(parent)
{
    this->setParent(parent);
    m_hDevHandle = IMV_NULL;

    _pushNum = 0;
    _camid = -2;
    _campicid = 0;
    _pushNum = 1;

    qRegisterMetaType<ImageInfo>("ImageInfo");
    connect(this, &DaHuaCamera::emit_sendFrame, this, &DaHuaCamera::ImageCallBackInner);
}

DaHuaCamera::~DaHuaCamera()
{
    if (m_hDevHandle)
    {
        IMV_DestroyHandle(m_hDevHandle);
        m_hDevHandle    = IMV_NULL;
    }
}

/////////
void DaHuaCamera::set_pushNum(int pushNum, int campicid, QDateTime timeCurrent)
{
    _pushNum = pushNum;
    _campicid = campicid;
    _time_daoWei = timeCurrent;
//    _isNewPush = true;
}

void DaHuaCamera::set_saveDir(QString dir, QString mergeDir)
{
    _saveDir = dir;
    _mergeDir = mergeDir;
}



void DaHuaCamera::dahuaImageCallBack(IMV_Frame* pFrame, void* pUser)
{
    DaHuaCamera *pcam = (DaHuaCamera*)pUser;
    QDateTime time_start = QDateTime::currentDateTime();

    pcam->_campicid++;
    if (pcam->_campicid > pcam->_pic_cnts){
        pcam->_campicid = 1;
        pcam->_pushNum += 1;

    }

    emit pcam->emit_sendFrame(pcam->_camid, pcam->_campicid, pcam->_pushNum,
                              pFrame->pData, pFrame->frameInfo.width, pFrame->frameInfo.height, time_start);
    emit pcam->emit_callbackCamid(pcam->_camid-1);
    printf("#### callback DaHua:  %d  %d__%d, %dx%d,  %s\n", pcam->_pushNum, pcam->_camid, pcam->_campicid, pFrame->frameInfo.width, pFrame->frameInfo.height, time_start.toString("yyyy-MM-dd ss:mm:ss.zzz").toLatin1().data());

}


void DaHuaCamera::ImageCallBackInner(int camid, int campicid, int pushNum, unsigned char* imgBuf, int w, int h, QDateTime time_start)
{
//    QtConcurrent::run([=]{

//        if (campicid == 1){
//            _timeFirst = QDateTime::currentDateTime();
//        }
//        ImageInfo pInfo;
//        pInfo.camid = camid;
//        pInfo.campicid = campicid;
//        pInfo.pushNum = pushNum;
//        pInfo.width = w;
//        pInfo.height = h;
//        pInfo.time_start = _timeFirst;

//        pInfo.SN = Int2QString(pInfo.pushNum);

//        pInfo.prefix = _saveFormats[_saveFormatIdx].arg(pInfo.SN).arg(_timeFirst.toString("hh-mm-ss")).arg(pInfo.campicid);
//        pInfo.src_path = QString("%1/%2_0_org.").arg(_saveDir).arg(pInfo.prefix) + _saveSuffix;
//        pInfo.dst_path = QString("%1/%2_0_org.").arg(_saveDir).arg(pInfo.prefix) + _saveSuffix;
//        pInfo.merge_path = QString("%1/%2_%3_0_org.").arg(_mergeDir).arg(pInfo.SN).arg(_timeFirst.toString("hh-mm-ss")) + _saveSuffix;

//        if(w < 10 || h < 10){ //warning...  || pFrameData->nImgSize < 10
//            printf(">>>>  loss stDisplayInfo.nWidth=%d\n", w);
//            printf(">>>>  loss stDisplayInfo.height=%d\n",h);
//            return;
//        }

//        int bodylength = w * h;
//        unsigned char* rgb24 = NULL;
//        rgb24 = (unsigned char*)malloc(bodylength);
//        memset(rgb24, 0, bodylength);

//        memcpy(rgb24, imgBuf, bodylength);
////        DxRaw8toRGB24((unsigned char*) imgBuf,rgb24, w, h, RAW2RGB_NEIGHBOUR,DX_PIXEL_COLOR_FILTER(0),false);
//        printf("  camid=%d, campicid=%d, DxRaw8toRGB24 completed !!!   %d x %d\n", camid, campicid, w, h);

//        cv::Mat srcImage = cv::Mat(h, w, CV_8UC1, rgb24);
//        pInfo.srcImage = srcImage.clone();

//        emit emit_imageInfo(pInfo);
//        if (_saveImgsNums == 0)
//            cv::imwrite(pInfo.src_path.toStdString(), srcImage);

////        QImage image(rgb24, w, h, QImage::Format_Grayscale8);
////        QString path = QString("%1/%2_0_org_1111.").arg(_saveDir).arg(pInfo.prefix) + _saveSuffix;
////        image.save(path, _saveSuffix.toUpper().toLatin1().data(), 100);

//        free(rgb24);
//        srcImage.release();

//        int time_total = time_start.msecsTo(QDateTime::currentDateTime());
//        printf("  ## %d  %d_%d  write_time=%d ms\n", pushNum, pInfo.camid, pInfo.campicid, time_total);
//        emit emit_log(QString(">>>>  %1__%2__%3    %4  saveImg: %5 ms")
//                               .arg(pInfo.SN).arg(pInfo.camid).arg(pInfo.campicid)
//                               .arg(time_start.toString("yyyy-MM-dd  hh-mm-ss.zzz")).arg(time_total));
//    });

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

//    cv::Mat srcImage(h, w, CV_8UC1, rgb24);
////    cv::Mat srcImage(h, w, CV_8UC1, rgb24.get());
//    pInfo.srcImage = srcImage.clone();

    emit emit_imageInfo(pInfo);
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


int DaHuaCamera::displayGigeDeviceInfo(bool isShow)
{
    IMV_DeviceList deviceInfoList;
    int ret = IMV_EnumDevices(&deviceInfoList, interfaceTypeAll);
//    if (IMV_OK != ret){
//        printf(" Enum DaHua Devices failed! ErrorCode[%d]\n", ret);
//    }
//    if (deviceInfoList.nDevNum < 1){
//        printf(" <DaHua> no camera\n");
//    }
    if (isShow){
        // 打印相机基本信息（序号,类型,制造商信息,型号,序列号,用户自定义ID,IP地>址）
        // Print camera info (Index, Type, Vendor, Model, Serial number, DeviceUserID, IP Address)
        displayDeviceInfo(deviceInfoList);
    }

    return deviceInfoList.nDevNum;
}

//////////////
int DaHuaCamera::initCamera(int camid, QString ip, int triggerMode)
{
    printf("---->>>> start init, camid=%d\n", camid);
    _camid = camid;
    _pic_cnts = 1;

    printf("[%s] getCameraHandle num:%d , ip=%s , ", __func__, camid, ip.toLatin1().data());
    int nnnRet = this->Open(ip.toLatin1().data(), triggerMode);  // 1:soft  other:line1
    if (nnnRet == IMV_OK){
        printf("DaHua  open successfully!\n");

        char* mode;
        if (_saveTriggerMode.toUpper() == "SOFTWARE"){
//            nnnRet = this->CommandExecute("TriggerSoftware");
            mode = "Software";
        }
        else{
//            nnnRet = this->CommandExecute("Line1");
            mode = "Line1";
        }
        nnnRet = IMV_SetEnumFeatureSymbol(m_hDevHandle, "TriggerSource", mode);

        if (nnnRet == IMV_OK){
            printf("[%s] trigger mode: %s\n", __func__, mode);
        }
        else{
            printf("[%s] trigger set failed! ErrorCode[%d]\n", __func__, nnnRet);
        }

        // 设置外触发为上升沿 RisingEdge （下降沿为 FallingEdge ）
        char* triggerActivation = "FallingEdge";
        nnnRet = IMV_SetEnumFeatureSymbol(m_hDevHandle, "TriggerActivation", triggerActivation);
        if (IMV_OK == nnnRet){
            printf("[%s] triggerActivation Success: %s\n", __func__, triggerActivation);
        }
        else{
            printf("[%s] triggerActivation Failed! ErrorCode[%d]\n", __func__, nnnRet);
            return nnnRet;
        }


        nnnRet = this->RegisterImageCallBack(dahuaImageCallBack, this);
        if (nnnRet == IMV_OK){
            printf("[%s] register callback success!\n", __func__);
        }
        this->StartGrabbing();
    }
    else{
        printf("DaHua  open failed!\n");
    }

    return nnnRet;
}




// ch:获取SDK版本号 | en:Get SDK Version
//int DaHuaCamera::GetSDKVersion()
//{
//    return MV_CC_GetSDKVersion();
//}

// ch:枚举设备 | en:Enumerate Device
int DaHuaCamera::EnumDevices(IMV_DeviceList *pDeviceList, unsigned int interfaceType)
{
    return IMV_EnumDevices(pDeviceList, interfaceType);
}


//// ch:判断设备是否可达 | en:Is the device accessible
//bool DaHuaCamera::IsDeviceAccessible(MV_CC_DEVICE_INFO* pstDevInfo, unsigned int nAccessMode)
//{
//    return MV_CC_IsDeviceAccessible(pstDevInfo, nAccessMode);
//}

static int setSoftTriggerConf(IMV_HANDLE devHandle,int softTrigger)
{
    int ret = IMV_OK;

    // 设置触发源为软触发
    // Set trigger source to Software
    if(softTrigger == 1)
    {
        ret = IMV_SetEnumFeatureSymbol(devHandle, "TriggerSource", "Software");
        if (IMV_OK != ret)
        {
            printf("Set triggerSource value failed! ErrorCode[%d]\n", ret);
            return ret;
        }
    }
    else{
        ret = IMV_SetEnumFeatureSymbol(devHandle, "TriggerSource", "Line1");
        if (IMV_OK != ret)
        {
            printf("Set triggerSource value failed! ErrorCode[%d]\n", ret);
            return ret;
        }
    }
    // 设置触发器
    // Set trigger selector to FrameStart
    ret = IMV_SetEnumFeatureSymbol(devHandle, "TriggerSelector", "FrameStart");
    if (IMV_OK != ret)
    {
        printf("Set triggerSelector value failed! ErrorCode[%d]\n", ret);
        return ret;
    }
    // 设置触发模式
    // Set trigger mode to On
    ret = IMV_SetEnumFeatureSymbol(devHandle, "TriggerMode", "On");
    if (IMV_OK != ret)
    {
        printf("Set triggerMode value failed! ErrorCode[%d]\n", ret);
        return ret;
    }

    return ret;
}


// ch:打开设备 | en:Open Device
int DaHuaCamera::Open(char *specifiedStr,int softTrigger)
{

    if (IMV_NULL == specifiedStr)
    {

        return IMV_ERROR;
    }


    int nRet  = IMV_CreateHandle(&m_hDevHandle,  modeByIPAddress, specifiedStr);
    if (IMV_OK != nRet)
    {

        return nRet;
    }

    nRet = IMV_Open(m_hDevHandle);
    if (IMV_OK != nRet)
    {

        IMV_DestroyHandle(m_hDevHandle);
        m_hDevHandle = IMV_NULL;
    }

    nRet =setSoftTriggerConf(m_hDevHandle,softTrigger);
    if (IMV_OK != nRet)
    {

        IMV_DestroyHandle(m_hDevHandle);
        m_hDevHandle = IMV_NULL;
    }

//     printf("fun=%s,line=%d,ret=%d,m_hDevHandle=%d\n",__func__,__LINE__,nRet,m_hDevHandle);
    return nRet;
}

// ch:关闭设备 | en:Close Device
int DaHuaCamera::Close()
{
    if (IMV_NULL == m_hDevHandle)
    {
        return IMV_ERROR;
    }

    IMV_Close(m_hDevHandle);

    int nRet = IMV_DestroyHandle(m_hDevHandle);
    m_hDevHandle = IMV_NULL;

    return nRet;
}

//// ch:判断相机是否处于连接状态 | en:Is The Device Connected
//bool DaHuaCamera::IsDeviceConnected()
//{
//    return MV_CC_IsDeviceConnected(m_hDevHandle);
//}


//IN IMV_HANDLE handle, IN IMV_FrameCallBack proc, IN void* pUser);
// ch:注册图像数据回调 | en:Register Image Data CallBack
int DaHuaCamera::RegisterImageCallBack(IMV_FrameCallBack proc, void* pUser)
{
   return IMV_AttachGrabbing(m_hDevHandle,(IMV_FrameCallBack) proc, pUser);
}

// ch:开启抓图 | en:Start Grabbing
int DaHuaCamera::StartGrabbing()
{

    return IMV_StartGrabbing(m_hDevHandle);
}

// ch:停止抓图 | en:Stop Grabbing
int DaHuaCamera::StopGrabbing()
{
    return IMV_StopGrabbing(m_hDevHandle);
}

int DaHuaCamera::CommandExecute(const char* strKey)
{

    return IMV_ExecuteCommandFeature(m_hDevHandle, strKey);
}

int DaHuaCamera::CommandExcuteLine(const char* strKey)
{

    return IMV_SetEnumFeatureSymbol(m_hDevHandle, "TriggerSource", strKey);
}
