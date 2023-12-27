#include "MVSCamera.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <sys/socket.h>
#include "qsqlquery.h"
#include <QMessageBox>
#include <QtConcurrent>

using namespace std;

#define ACQ_BUFFER_NUM  5

extern QString _cpp2Dir;

// ch:像素排列由RGB转为BGR | en:Convert pixel arrangement from RGB to BGR
void RGB2BGR( unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight )
{
    if ( NULL == pRgbData )
    {
        return;
    }

    // red和blue数据互换
    for (unsigned int j = 0; j < nHeight; j++)
    {
        for (unsigned int i = 0; i < nWidth; i++)
        {
            unsigned char red = pRgbData[j * (nWidth * 3) + i * 3];
            pRgbData[j * (nWidth * 3) + i * 3]     = pRgbData[j * (nWidth * 3) + i * 3 + 2];
            pRgbData[j * (nWidth * 3) + i * 3 + 2] = red;
        }
    }
}

// ch:帧数据转换为Mat格式图片并保存 | en:Convert data stream to Mat format then save image
cv::Mat Convert2Mat(MV_FRAME_OUT_INFO_EX* pstImageInfo, unsigned char * pData)
{
    cv::Mat srcImage;

    if ( PixelType_Gvsp_Mono8 == pstImageInfo->enPixelType )                // Mono8类型
    {
        srcImage = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC1, pData);
    }
    else if ( PixelType_Gvsp_RGB8_Packed == pstImageInfo->enPixelType )     // RGB8类型
    {
        // Mat像素排列格式为BGR，需要转换
        RGB2BGR(pData, pstImageInfo->nWidth, pstImageInfo->nHeight);
        srcImage = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC3, pData);
    }
    else
    {
        /* Bayer 格式转换mat格式的方法:
        1. 使用相机句柄销毁前 调用 MV_CC_ConvertPixelType 将PixelType_Gvsp_BayerRG8 等Bayer格式转换成 PixelType_Gvsp_BGR8_Packed
        2. 参考上面 将BGR转换为 mat格式
        */

        printf("Unsupported pixel format\n");
    }

    if ( NULL == srcImage.data ){
        printf("Creat Mat failed.\n");
    }

    return srcImage;
}



MVSCamera::MVSCamera(QWidget* parent):QObject(parent)
{
    this->setParent(parent);
    m_hDevHandle = MV_NULL;

    _camid = -2;
    _campicid = 0;
    _pic_cnts = 0;
    _pushNum = 1;
    _isNewPush = false;

    connect(this, &MVSCamera::emit_sendFrame, this, &MVSCamera::ImageCallBackInner);
}

MVSCamera::~MVSCamera()
{
    if (m_hDevHandle)
    {
        // 停止取流
        MV_CC_StopGrabbing(m_hDevHandle);

        // 关闭设备
        MV_CC_CloseDevice(m_hDevHandle);

        // 销毁句柄
        MV_CC_DestroyHandle(m_hDevHandle);
        m_hDevHandle    = MV_NULL;
    }
}

int MVSCamera::closeCameras()
{
    try {
        // 停止取流
        MV_CC_StopGrabbing(m_hDevHandle);

        // 关闭设备
        MV_CC_CloseDevice(m_hDevHandle);

        // 销毁句柄
        MV_CC_DestroyHandle(m_hDevHandle);
        m_hDevHandle    = MV_NULL;
    }  catch (...) {
        QMessageBox::warning(nullptr, "错误！", "关闭相机失败！");
    }
}


int MVSCamera::judgeCamid()
{
    printf("  camera.camid=%d\n", _camid);
    return _camid;
}


/////////
void MVSCamera::set_pushNum(int pushNum, int campicid, QDateTime timeCurrent)
{
    _pushNum = pushNum;
    _campicid = campicid;
    _time_daoWei = timeCurrent;
//    _isNewPush = true;
}

void MVSCamera::set_saveDir(QString dir, QString mergeDir)
{
    _saveDir = dir;
    _mergeDir = mergeDir;
}

/////////////
void MVSCamera::mvsImageCallBack(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
    MVSCamera *pcam = (MVSCamera*)pUser;
    QDateTime time_start = QDateTime::currentDateTime();

    pcam->_campicid++;
    if (pcam->_campicid > pcam->_pic_cnts){
//        if (pcam->_timeStart.msecsTo(QDateTime::currentDateTime()) < 2500)
//            return;

        pcam->_campicid = 1;
        pcam->_pushNum += 1;

    }
//    else if (pcam->_camid == pcam->_pic_cnts){
//        if (pcam->_camid != 0 && pcam->_camid != 4)
//            return;
//    }

    printf("#### callback: camid=%d, campicid=%d ---- enPixelType=%ld\n", pcam->_camid, pcam->_campicid, pFrameInfo->enPixelType);

    emit pcam->emit_sendFrame(pcam->_camid, pcam->_campicid, pcam->_pushNum, pFrameInfo, pData, time_start);
}

//////////////
void MVSCamera::ImageCallBackInner(int camid, int campicid, int pushNum, MV_FRAME_OUT_INFO_EX* pFrameInfo, uchar* data, QDateTime time_start)
{
    QtConcurrent::run([=]{

        QDateTime time;
        if (campicid == 1){
            _timeStart = QDateTime::currentDateTime();
            _timeCur = _timeStart.toString("hh-mm-ss");

        }
        else if (campicid == 3){
            time = QDateTime::currentDateTime();
        }

        int pushNum_new = pushNum;
        int campicid_new = campicid;
//        if (_timeStart.msecsTo(time_start) > 2500){
//            _pushNum++;
//            _campicid = 1;

//            pushNum_new = _pushNum;
//            campicid_new = _campicid;
//            if (_campicid == 1){
//                _timeStart = QDateTime::currentDateTime();
//                _timeCur = _timeStart.toString("hh-mm-ss");

//            }
//            else if (campicid == 3){
//                time = QDateTime::currentDateTime();
//            }
//        }

        ImageInfo pInfo;
        pInfo.camid = camid;
        pInfo.width = pFrameInfo->nWidth;
        pInfo.height = pFrameInfo->nHeight;

        pInfo.campicid = campicid_new;
        pInfo.pushNum = pushNum_new;
        pInfo.SN = Int2QString(pInfo.pushNum);

        pInfo.prefix = _saveFormats[_saveFormatIdx].arg(pInfo.SN).arg(_timeCur).arg(pInfo.campicid);
        pInfo.src_path = QString("%1/%2_0_org.").arg(_saveDir).arg(pInfo.prefix) + _saveSuffix;
        pInfo.dst_path = QString("%1/%2_0_org.").arg(_saveDir).arg(pInfo.prefix) + _saveSuffix;
        pInfo.merge_path = QString("%1/%2_%3_0_org.").arg(_mergeDir).arg(pInfo.SN).arg(_timeCur) + _saveSuffix;
        if (campicid == 3)
            pInfo.time_start = time;
        else
            pInfo.time_start = _timeStart;

        writeImage(pInfo, pFrameInfo, data);
    //    QtConcurrent::run(this, &MVSCamera::writeImage, pInfo, pFrameInfo, data);
        int time_total = time_start.msecsTo(QDateTime::currentDateTime());
        printf("  ## %d_%d  write_time=%d ms\n", pInfo.camid, pInfo.campicid, time_total);
        emit emit_recvCallback(QString(">>>>  %1__%2__%3    %4  saveImg: %5 ms")
                               .arg(pInfo.SN).arg(pInfo.camid).arg(pInfo.campicid)
                               .arg(time_start.toString("yyyy-MM-dd  hh-mm-ss.zzz")).arg(time_total));
    });

}

void MVSCamera::writeImage(ImageInfo& pInfo, MV_FRAME_OUT_INFO_EX* pFrameInfo, uchar* data)
{
//    printf("  pushNum=%d, cam->id=%d, cam->picid=%d, image.enPixelType=%ld\n", pInfo.pushNum, pInfo.camid, pInfo.campicid, pFrameInfo->enPixelType);

//    // 方式一：
//    cv::Mat srcImage = Convert2Mat(pFrameInfo, data);
//    pInfo.srcImage = srcImage;
//    cv::imwrite(pInfo.src_path.toStdString(), srcImage);

    // 方式二：像素格式转换
    unsigned char *pConvertData = NULL;
    pConvertData = (unsigned char*)malloc(pInfo.width * pInfo.height * 1);

    MV_CC_PIXEL_CONVERT_PARAM CvtParam;
    // 从上到下依次是：图像宽，图像高，输入数据缓存，输入数据大小，源像素格式，
    // 目标像素格式，输出数据缓存，提供的输出缓冲区大小
    CvtParam.nWidth           = pFrameInfo->nWidth;
    CvtParam.nHeight          = pFrameInfo->nHeight;
    CvtParam.pSrcData         = data;
    CvtParam.nSrcDataLen      = pFrameInfo->nFrameLen;
    CvtParam.enSrcPixelType   = pFrameInfo->enPixelType;
    CvtParam.enDstPixelType   = PixelType_Gvsp_Mono8;
    CvtParam.pDstBuffer       = pConvertData;
//    CvtParam.nDstBufferSize   = pFrameInfo->nWidth * pFrameInfo->nHeight *  1;
//    printf("  image.srcLen=%d\n", pFrameInfo->nFrameLen);
//    printf("  image.dstLen=%d\n", CvtParam.nDstLen);

    MV_CC_ConvertPixelType(m_hDevHandle, &CvtParam);
    cv::Mat srcImage = cv::Mat(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC1, pConvertData);
//    cvtColor(srcImage, srcImage, cv::COLOR_RGB2BGR);

//    pInfo.srcImage = srcImage.clone();
    emit emit_imageInfo(pInfo);

    cv::imwrite(pInfo.src_path.toStdString(), srcImage);

//    printf("  Save image Success\n");
    free(pConvertData);
    srcImage.release();

//    // 方式三：填充存图参数
//    int nRet = MV_OK;
//    unsigned char *pDataForSaveImage = NULL;
//    pDataForSaveImage = (unsigned char*)malloc(pInfo.width * pInfo.height * 4 + 2048);

//    MV_SAVE_IMAGE_PARAM_EX stSaveParam;
//    memset(&stSaveParam, 0, sizeof(MV_SAVE_IMAGE_PARAM_EX));
//    // 从上到下依次是：输出图片格式，输入数据的像素格式，提供的输出缓冲区大小，图像宽，
//    // 图像高，输入数据缓存，输出图片缓存，JPG编码质量
//    stSaveParam.enImageType = MV_Image_Jpeg;
//    stSaveParam.enPixelType = pFrameInfo->enPixelType; // PixelType_Gvsp_RGB8_Packed; //pFrameInfo->enPixelType;  35127316  17301505
//    stSaveParam.nBufferSize = pInfo.width * pInfo.height * 4 + 2048;
//    stSaveParam.nWidth      = pInfo.width;
//    stSaveParam.nHeight     = pInfo.height;
//    stSaveParam.pData       = data;
//    stSaveParam.nDataLen    = pFrameInfo->nFrameLen;
//    stSaveParam.pImageBuffer = pDataForSaveImage;
//    stSaveParam.nJpgQuality = 80;

//    nRet = MV_CC_SaveImageEx2(m_hDevHandle, &stSaveParam);
//    if(MV_OK != nRet){
//        printf("Failed in MV_CC_SaveImage, nRet[%x]\n", nRet);
//    }
//    printf("  image.imageLen=%d\n", stSaveParam.nImageLen);

//    cv::Mat srcImage = cv::Mat(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3, pDataForSaveImage);
////    cvtColor(srcImage, srcImage, cv::COLOR_RGB2BGR);

//    pInfo.srcImage = srcImage.clone();
////    emit emit_imageInfo(pInfo);

//    cv::imwrite(pInfo.dst_path.toStdString(), srcImage);

//    FILE* fp = fopen(pInfo.src_path.toLatin1().data(), "wb");
//    fwrite(pDataForSaveImage, 1, stSaveParam.nImageLen, fp);
//    fclose(fp);

//    free(pDataForSaveImage);
//    printf("  Save image Success\n");


}


////////////////////
int MVSCamera::initCamera(int camid, MV_CC_DEVICE_INFO* pDeviceInfo)
{
    _camid = camid;
    switch (camid){
    case 1:
    case 4:
        _pic_cnts = 7;
        break;
    case 2:
    case 3:
    case 5:
    case 6:
        _pic_cnts = 13;
        break;
    case 7:
        _pic_cnts = 22;
        break;
    }

    int nRet = MV_OK;

    //
    nRet =  this->Open(pDeviceInfo);
    if (MV_OK != nRet){
        printf("  camid=%d Open Fail: %d \n", camid, nRet);
    }
    else
        printf("  camid=%d Open Success: %d \n", camid, nRet);

    // ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
    if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE){
        unsigned int nPacketSize = 0;
        nRet = this->GetOptimalPacketSize(&nPacketSize);
        if (nRet == MV_OK) {
            nRet = this->SetIntValue("GevSCPSPacketSize", 9000);
            if(nRet != MV_OK) {
                 printf("  Error: Set Packet Size fail!\n");
            }
            else{
                printf("  best Packet Size: %d\n", nPacketSize);
                printf("  current Packet Size: 9000\n");
            }
        }
        else {
             printf("  Error: Get Packet Size fail!\n");
        }
    }

//    nRet = MV_CC_SetBoolValue(m_hDevHandle, "AcquisitionFrameRateEnable", false);
//    if (MV_OK != nRet){
//        printf("set AcquisitionFrameRateEnable fail! nRet [%x]\n", nRet);
//    }

    this->SetEnumValue("AcquisitionMode", MV_ACQ_MODE_CONTINUOUS);

    this->SetEnumValue("TriggerMode", MV_TRIGGER_MODE_ON);

    if (_saveTriggerMode.toUpper() == "SOFTWARE"){
        this->SetEnumValue("TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE);
    }
    else{
        this->SetEnumValue("TriggerSource", MV_TRIGGER_SOURCE_LINE0);
    }
    printf("  trigger mode: %s\n", _saveTriggerMode.toUpper().toLatin1().data());


    nRet = this->RegisterImageCallBack(mvsImageCallBack, this);
    if (MV_OK != nRet){
        printf("  Error: register callback fail\n");
    }
    else
        printf("  Success: register callback success\n");


    nRet = this->StartGrabbing();
    if (MV_OK != nRet){
        printf("  Error: Start grabbing fail\n");
    }
    else
        printf("  Success: Start grabbing success\n");

    return MV_OK;
}


bool MVSCamera::PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo, int& net, QString& ipStr)
{
    if (NULL == pstMVDevInfo)
    {
        printf("  The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
    {
        int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
        net = nIp3;
        ipStr = QString("%1.%2.%3.%4").arg(nIp1).arg(nIp2).arg(nIp3).arg(nIp4);

        // ch:打印当前相机ip和用户自定义名字 | en:print current ip and user defined name
        printf("  Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName);
        printf("  CurrentIp: %d.%d.%d.%d\n" , nIp1, nIp2, nIp3, nIp4);
        printf("  UserDefinedName: %s\n\n" , pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    }
    else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        printf("  Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chModelName);
        printf("  UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
    }
    else
    {
        printf("  Not support.\n");
    }

    return true;
}




// ch:获取SDK版本号 | en:Get SDK Version
int MVSCamera::GetSDKVersion()
{
    return MV_CC_GetSDKVersion();
}

// ch:枚举设备 | en:Enumerate Device
int MVSCamera::EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList)
{
    return MV_CC_EnumDevices(nTLayerType, pstDevList);
}

// ch:判断设备是否可达 | en:Is the device accessible
bool MVSCamera::IsDeviceAccessible(MV_CC_DEVICE_INFO* pstDevInfo, unsigned int nAccessMode)
{
    return MV_CC_IsDeviceAccessible(pstDevInfo, nAccessMode);
}

// ch:打开设备 | en:Open Device
int MVSCamera::Open(MV_CC_DEVICE_INFO* pstDeviceInfo)
{
    if (MV_NULL == pstDeviceInfo)
    {
        return MV_E_PARAMETER;
    }

    if (m_hDevHandle)
    {
        printf("  #### Has Been Used!\n");
        return MV_E_CALLORDER;
    }

    int nRet  = MV_CC_CreateHandle(&m_hDevHandle, pstDeviceInfo);
    if (MV_OK != nRet)
    {
        printf("  #### No Opened!\n");
        return nRet;
    }

    nRet = MV_CC_OpenDevice(m_hDevHandle);
    if (MV_OK != nRet)
    {
        MV_CC_DestroyHandle(m_hDevHandle);
        m_hDevHandle = MV_NULL;
    }

    return nRet;
}

// ch:关闭设备 | en:Close Device
int MVSCamera::Close()
{
    if (MV_NULL == m_hDevHandle)
    {
        return MV_E_HANDLE;
    }

    MV_CC_CloseDevice(m_hDevHandle);

    int nRet = MV_CC_DestroyHandle(m_hDevHandle);
    m_hDevHandle = MV_NULL;

    return nRet;
}

// ch:判断相机是否处于连接状态 | en:Is The Device Connected
bool MVSCamera::IsDeviceConnected()
{
    return MV_CC_IsDeviceConnected(m_hDevHandle);
}

// ch:注册图像数据回调 | en:Register Image Data CallBack
int MVSCamera::RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser)
{
   // return MV_CC_RegisterImageCallBackForBGR(m_hDevHandle, cbOutput, pUser);
     return MV_CC_RegisterImageCallBackForRGB(m_hDevHandle, cbOutput, pUser);
}

// ch:开启抓图 | en:Start Grabbing
int MVSCamera::StartGrabbing()
{
    return MV_CC_StartGrabbing(m_hDevHandle);
}

// ch:停止抓图 | en:Stop Grabbing
int MVSCamera::StopGrabbing()
{
    return MV_CC_StopGrabbing(m_hDevHandle);
}

// ch:主动获取一帧图像数据 | en:Get one frame initiatively
int MVSCamera::GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec)
{
    return MV_CC_GetImageBuffer(m_hDevHandle, pFrame, nMsec);
}

// ch:释放图像缓存 | en:Free image buffer
int MVSCamera::FreeImageBuffer(MV_FRAME_OUT* pFrame)
{
    return MV_CC_FreeImageBuffer(m_hDevHandle, pFrame);
}

// ch:设置显示窗口句柄 | en:Set Display Window Handle
int MVSCamera::DisplayOneFrame(MV_DISPLAY_FRAME_INFO* pDisplayInfo)
{
    return MV_CC_DisplayOneFrame(m_hDevHandle, pDisplayInfo);
}

// ch:设置SDK内部图像缓存节点个数 | en:Set the number of the internal image cache nodes in SDK
int MVSCamera::SetImageNodeNum(unsigned int nNum)
{
    return MV_CC_SetImageNodeNum(m_hDevHandle, nNum);
}

// ch:获取设备信息 | en:Get device information
int MVSCamera::GetDeviceInfo(MV_CC_DEVICE_INFO* pstDevInfo)
{
    return MV_CC_GetDeviceInfo(m_hDevHandle, pstDevInfo);
}

// ch:获取GEV相机的统计信息 | en:Get detect info of GEV camera
int MVSCamera::GetGevAllMatchInfo(MV_MATCH_INFO_NET_DETECT* pMatchInfoNetDetect)
{
    if (MV_NULL == pMatchInfoNetDetect)
    {
        return MV_E_PARAMETER;
    }

    MV_CC_DEVICE_INFO stDevInfo = {0};
    GetDeviceInfo(&stDevInfo);
    if (stDevInfo.nTLayerType != MV_GIGE_DEVICE)
    {
        return MV_E_SUPPORT;
    }

    MV_ALL_MATCH_INFO struMatchInfo = {0};

    struMatchInfo.nType = MV_MATCH_TYPE_NET_DETECT;
    struMatchInfo.pInfo = pMatchInfoNetDetect;
    struMatchInfo.nInfoSize = sizeof(MV_MATCH_INFO_NET_DETECT);
    memset(struMatchInfo.pInfo, 0, sizeof(MV_MATCH_INFO_NET_DETECT));

    return MV_CC_GetAllMatchInfo(m_hDevHandle, &struMatchInfo);
}

// ch:获取U3V相机的统计信息 | en:Get detect info of U3V camera
int MVSCamera::GetU3VAllMatchInfo(MV_MATCH_INFO_USB_DETECT* pMatchInfoUSBDetect)
{
    if (MV_NULL == pMatchInfoUSBDetect)
    {
        return MV_E_PARAMETER;
    }

    MV_CC_DEVICE_INFO stDevInfo = {0};
    GetDeviceInfo(&stDevInfo);
    if (stDevInfo.nTLayerType != MV_USB_DEVICE)
    {
        return MV_E_SUPPORT;
    }

    MV_ALL_MATCH_INFO struMatchInfo = {0};

    struMatchInfo.nType = MV_MATCH_TYPE_USB_DETECT;
    struMatchInfo.pInfo = pMatchInfoUSBDetect;
    struMatchInfo.nInfoSize = sizeof(MV_MATCH_INFO_USB_DETECT);
    memset(struMatchInfo.pInfo, 0, sizeof(MV_MATCH_INFO_USB_DETECT));

    return MV_CC_GetAllMatchInfo(m_hDevHandle, &struMatchInfo);
}

// ch:获取和设置Int型参数，如 Width和Height，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
// en:Get Int type parameters, such as Width and Height, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int MVSCamera::GetIntValue(IN const char* strKey, OUT MVCC_INTVALUE_EX *pIntValue)
{
    return MV_CC_GetIntValueEx(m_hDevHandle, strKey, pIntValue);
}

int MVSCamera::SetIntValue(IN const char* strKey, IN int64_t nValue)
{
    return MV_CC_SetIntValueEx(m_hDevHandle, strKey, nValue);
}

// ch:获取和设置Enum型参数，如 PixelFormat，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
// en:Get Enum type parameters, such as PixelFormat, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int MVSCamera::GetEnumValue(IN const char* strKey, OUT MVCC_ENUMVALUE *pEnumValue)
{
    return MV_CC_GetEnumValue(m_hDevHandle, strKey, pEnumValue);
}

int MVSCamera::SetEnumValue(IN const char* strKey, IN unsigned int nValue)
{
    return MV_CC_SetEnumValue(m_hDevHandle, strKey, nValue);
}

int MVSCamera::SetEnumValueByString(IN const char* strKey, IN const char* sValue)
{
    return MV_CC_SetEnumValueByString(m_hDevHandle, strKey, sValue);
}

// ch:获取和设置Float型参数，如 ExposureTime和Gain，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
// en:Get Float type parameters, such as ExposureTime and Gain, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int MVSCamera::GetFloatValue(IN const char* strKey, OUT MVCC_FLOATVALUE *pFloatValue)
{
    return MV_CC_GetFloatValue(m_hDevHandle, strKey, pFloatValue);
}

int MVSCamera::SetFloatValue(IN const char* strKey, IN float fValue)
{
    return MV_CC_SetFloatValue(m_hDevHandle, strKey, fValue);
}

// ch:获取和设置Bool型参数，如 ReverseX，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
// en:Get Bool type parameters, such as ReverseX, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int MVSCamera::GetBoolValue(IN const char* strKey, OUT bool *pbValue)
{
    return MV_CC_GetBoolValue(m_hDevHandle, strKey, pbValue);
}

int MVSCamera::SetBoolValue(IN const char* strKey, IN bool bValue)
{
    return MV_CC_SetBoolValue(m_hDevHandle, strKey, bValue);
}

// ch:获取和设置String型参数，如 DeviceUserID，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件UserSetSave
// en:Get String type parameters, such as DeviceUserID, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int MVSCamera::GetStringValue(IN const char* strKey, MVCC_STRINGVALUE *pStringValue)
{
    return MV_CC_GetStringValue(m_hDevHandle, strKey, pStringValue);
}

int MVSCamera::SetStringValue(IN const char* strKey, IN const char* strValue)
{
    return MV_CC_SetStringValue(m_hDevHandle, strKey, strValue);
}

// ch:执行一次Command型命令，如 UserSetSave，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
// en:Execute Command once, such as UserSetSave, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int MVSCamera::CommandExecute(IN const char* strKey)
{
    return MV_CC_SetCommandValue(m_hDevHandle, strKey);
}

// ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
int MVSCamera::GetOptimalPacketSize(unsigned int* pOptimalPacketSize)
{
    if (MV_NULL == pOptimalPacketSize)
    {
        return MV_E_PARAMETER;
    }

    int nRet = MV_CC_GetOptimalPacketSize(m_hDevHandle);
    if (nRet < MV_OK)
    {
        return nRet;
    }

    *pOptimalPacketSize = (unsigned int)nRet;

    return MV_OK;
}

// ch:注册消息异常回调 | en:Register Message Exception CallBack
int MVSCamera::RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser),void* pUser)
{
    return MV_CC_RegisterExceptionCallBack(m_hDevHandle, cbException, pUser);
}

// ch:注册单个事件回调 | en:Register Event CallBack
int MVSCamera::RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO * pEventInfo, void* pUser), void* pUser)
{
    return MV_CC_RegisterEventCallBackEx(m_hDevHandle, pEventName, cbEvent, pUser);
}

// ch:强制IP | en:Force IP
int MVSCamera::ForceIp(unsigned int nIP, unsigned int nSubNetMask, unsigned int nDefaultGateWay)
{
    return MV_GIGE_ForceIpEx(m_hDevHandle, nIP, nSubNetMask, nDefaultGateWay);
}

// ch:配置IP方式 | en:IP configuration method
int MVSCamera::SetIpConfig(unsigned int nType)
{
    return MV_GIGE_SetIpConfig(m_hDevHandle, nType);
}

// ch:设置网络传输模式 | en:Set Net Transfer Mode
int MVSCamera::SetNetTransMode(unsigned int nType)
{
    return MV_GIGE_SetNetTransMode(m_hDevHandle, nType);
}

// ch:像素格式转换 | en:Pixel format conversion
int MVSCamera::ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM* pstCvtParam)
{
    return MV_CC_ConvertPixelType(m_hDevHandle, pstCvtParam);
}

// ch:保存图片 | en:save image
int MVSCamera::SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam)
{
    return MV_CC_SaveImageEx2(m_hDevHandle, pstParam);
}



