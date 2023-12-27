/************************************************************************/
/* 海康相机： 以C++接口为基础，对常用函数进行二次封装，方便用户使用                */
/************************************************************************/

#ifndef _MV_CAMERA_H_
#define _MV_CAMERA_H_

#include "MVSinclude/MvCameraControl.h"
#include "commonZack.h"
#include <string.h>
#include <QObject>
#include <opencv2/opencv.hpp>

#ifndef MV_NULL
#define MV_NULL    0
#endif

typedef struct __ImageInfo1__
{
    int camid;     /**< 相机ID 1 2 3 4  */
    int campicid;  /**< 相机拍照次数  */
    int pushNum;   /**< 节拍数  */
    int label;     /**< 返回结果 0 NG 1 OK 2 NULL  */
    int height;
    int width;
    int channels;
    QString time;
    QString src_path;          /**< 原始图像保存路径  */
    QString dst_path;          /**< 结果图像保存路径  */
    unsigned char* srcData;    /**< 图像数据   */
    int srcDataLens;
}ImageInfo_buf;

typedef struct __ImageInfo__
{
    int camid;     /**< 相机ID 1 2 3 4  */
    QString SN;
    int campicid;  /**< 相机拍照次数  */
    int pushNum;   /**< 节拍数  */
    int label;     /**< 返回结果 0 NG 1 OK 2 NULL  */
    int height;
    int width;
    int channels;
    QString prefix;
    QString src_path;          /**< 原始图像保存路径  */
    QString dst_path;          /**< 结果图像保存路径  */
//    cv::Mat srcImage;          /**< 图像数据   */

    unsigned char*	srcData;
//    std::shared_ptr<unsigned char> data;
    QString merge_path;
    QDateTime time_start;

}ImageInfo;


class MVSCamera : public QObject
{
    Q_OBJECT

public:
    explicit MVSCamera(QWidget* parent=0);
    ~MVSCamera();

    static void mvsImageCallBack(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser);

    // 初始化
    int initCamera(int camid, MV_CC_DEVICE_INFO* pDeviceInfo);

    int closeCameras();

    bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo, int& net, QString& ipStr);

    int judgeCamid();

    void set_pushNum(int pushNum, int campicid, QDateTime timeCurrent);
    void set_saveDir(QString dir, QString mergeDir);

    unsigned short BLEndianUshort(unsigned short value);
    void writeImage(ImageInfo& pInfo, MV_FRAME_OUT_INFO_EX* pFrameInfo, uchar* data);

private slots:
    void ImageCallBackInner(int camid, int campicid, int pushNum, MV_FRAME_OUT_INFO_EX* pFrameInfo, uchar* data, QDateTime time_start);

signals:
    void emit_sendFrame(int camid, int campicid, int pushNum, MV_FRAME_OUT_INFO_EX* pFrameInfo, uchar* data, QDateTime time_start);
    void emit_recvCallback(QString info);
    void emit_imageInfo(ImageInfo pInfo); // , unsigned char* buf);

    void emit_offline(int camid);

public:
    int _camid;
    int _campicid;
    int _pic_cnts;
    int _pushNum;
    QString _saveDir;
    QString _mergeDir;

    QString _timeCur;
    QDateTime _timeStart;
    QDateTime _time_daoWei;
    bool _isNewPush;

    // ch:获取SDK版本号 | en:Get SDK Version
    static int GetSDKVersion();

    // ch:枚举设备 | en:Enumerate Device
    static int EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList);

    // ch:判断设备是否可达 | en:Is the device accessible
    static bool IsDeviceAccessible(MV_CC_DEVICE_INFO* pstDevInfo, unsigned int nAccessMode);

    // ch:打开设备 | en:Open Device
    int Open(MV_CC_DEVICE_INFO* pstDeviceInfo);

    // ch:关闭设备 | en:Close Device
    int Close();

    // ch:判断相机是否处于连接状态 | en:Is The Device Connected
    bool IsDeviceConnected();

    // ch:注册图像数据回调 | en:Register Image Data CallBack
    int RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser);

    // ch:开启抓图 | en:Start Grabbing
    int StartGrabbing();

    // ch:停止抓图 | en:Stop Grabbing
    int StopGrabbing();

    // ch:主动获取一帧图像数据 | en:Get one frame initiatively
    int GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec);

    // ch:释放图像缓存 | en:Free image buffer
    int FreeImageBuffer(MV_FRAME_OUT* pFrame);

    // ch:显示一帧图像 | en:Display one frame image
    int DisplayOneFrame(MV_DISPLAY_FRAME_INFO* pDisplayInfo);

    // ch:设置SDK内部图像缓存节点个数 | en:Set the number of the internal image cache nodes in SDK
    int SetImageNodeNum(unsigned int nNum);

    // ch:获取设备信息 | en:Get device information
    int GetDeviceInfo(MV_CC_DEVICE_INFO* pstDevInfo);

    // ch:获取GEV相机的统计信息 | en:Get detect info of GEV camera
    int GetGevAllMatchInfo(MV_MATCH_INFO_NET_DETECT* pMatchInfoNetDetect);

    // ch:获取U3V相机的统计信息 | en:Get detect info of U3V camera
    int GetU3VAllMatchInfo(MV_MATCH_INFO_USB_DETECT* pMatchInfoUSBDetect);

    // ch:获取和设置Int型参数，如 Width和Height，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
    // en:Get Int type parameters, such as Width and Height, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int GetIntValue(IN const char* strKey, OUT MVCC_INTVALUE_EX *pIntValue);
    int SetIntValue(IN const char* strKey, IN int64_t nValue);

    // ch:获取和设置Enum型参数，如 PixelFormat，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
    // en:Get Enum type parameters, such as PixelFormat, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int GetEnumValue(IN const char* strKey, OUT MVCC_ENUMVALUE *pEnumValue);
    int SetEnumValue(IN const char* strKey, IN unsigned int nValue);
    int SetEnumValueByString(IN const char* strKey, IN const char* sValue);

    // ch:获取和设置Float型参数，如 ExposureTime和Gain，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
    // en:Get Float type parameters, such as ExposureTime and Gain, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int GetFloatValue(IN const char* strKey, OUT MVCC_FLOATVALUE *pFloatValue);
    int SetFloatValue(IN const char* strKey, IN float fValue);

    // ch:获取和设置Bool型参数，如 ReverseX，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
    // en:Get Bool type parameters, such as ReverseX, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int GetBoolValue(IN const char* strKey, OUT bool *pbValue);
    int SetBoolValue(IN const char* strKey, IN bool bValue);

    // ch:获取和设置String型参数，如 DeviceUserID，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件UserSetSave
    // en:Get String type parameters, such as DeviceUserID, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int GetStringValue(IN const char* strKey, MVCC_STRINGVALUE *pStringValue);
    int SetStringValue(IN const char* strKey, IN const char * strValue);

    // ch:执行一次Command型命令，如 UserSetSave，详细内容参考SDK安装目录下的 MvCameraNode.xlsx 文件
    // en:Execute Command once, such as UserSetSave, for details please refer to MvCameraNode.xlsx file under SDK installation directory
    int CommandExecute(IN const char* strKey);

    // ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
    int GetOptimalPacketSize(unsigned int* pOptimalPacketSize);

    // ch:注册消息异常回调 | en:Register Message Exception CallBack
    int RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser), void* pUser);

    // ch:注册单个事件回调 | en:Register Event CallBack
    int RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO * pEventInfo, void* pUser), void* pUser);

    // ch:强制IP | en:Force IP
    int ForceIp(unsigned int nIP, unsigned int nSubNetMask, unsigned int nDefaultGateWay);

    // ch:配置IP方式 | en:IP configuration method
    int SetIpConfig(unsigned int nType);

    // ch:设置网络传输模式 | en:Set Net Transfer Mode
    int SetNetTransMode(unsigned int nType);

    // ch:像素格式转换 | en:Pixel format conversion
    int ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM* pstCvtParam);

    // ch:保存图片 | en:save image
    int SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam);

    // ch:保存图片为文件 | en:Save the image as a file
    //int SaveImageToFile(MV_SAVE_IMG_TO_FILE_PARAM* pstParam);

private:

    void*               m_hDevHandle;



};


#endif//_MV_CAMERA_H_
