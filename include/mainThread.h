#ifndef MainThread_H
#define MainThread_H


#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "Quectel_Infer.h"
#include "commonZack.h"
#include "MVSCamera.h"
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QMutex>
#include "bearing.h"

using namespace cv;



class MainThread: public QThread
{
    Q_OBJECT

public:
    MainThread(QWidget* parent, int index, QString jsonPath, bool isAI=true, bool isWaitNo1=false, int size=10, int mergeSize=9);
    ~MainThread();

    void init_OCR(QString text1="608z", QString text2="GRY");
    void closed();

    void transform_data(ImageInfo pic_info);
    void transform_preParams(int camid, QList<int> pre_params);
    void transform_preROI(int camid, QList<int> pre_ROI);
    void transform_postParams(int camid, QMap<QString, QList<int>> labels_params);

    QImage drawChineseText(const QImage& q_image, QList<QString> texts, QList<QList<int>> pts, QList<int> fontSizes, QList<QColor> colors);

protected:
    void run();

private:
    void init_inferImage(int index, Quectel_Infer* _infer);

    void inference(Mat& srcImage, Mat& dstImage, int& predLabel, bool isShow, ImageInfo img_info, float& sum_area);
//    void startInference(Mat& dstImage, string& predLabel, bool isShow);

    bool merge(ImageInfo img_info, cv::Mat dst, int predLabel, float sum_area);
    bool merge_func(ImageInfo img_info, cv::Mat dst, int predLabel, float sum_area);

    QImage MergeImages_qimage(QList<QImage> images, int rows, int cols, int row_spacing, int col_spacing, int channels=3, int result=0, bool showLabel=false, QList<int> labels={});

    QImage drawImages_qimage(int camid, QImage images, int channels, int result);

    void generate_mergeNG(QImage mergeImg, int result, QString path);

signals:
    void emit_initYes(int initState, float loaded_time);
    void emit_initChar(int camid, QList<QString> keys);
    void emit_log(QString text);

    void emit_results(int camid, int campicid, QImage dstImage, int result, QString merge_path, float sum_area);
    void emit_input_data(int camid, Bearing_input_data inputData);

    void emit_labels_show(int camid, CamRusultRecv labels_show_tmp);


public slots:
    void mainThread_input_data_received(int camid, Bearing_input_data inputData);

    void mainThread_init_photo_store_received();

public:
    bool _isStop;
//    CircularQueue<ImageInfo> _queue = CircularQueue<ImageInfo>(25+2);
    QQueue<ImageInfo> _queue;

    Quectel_Infer* _quectel_infer;
    QString _jsonPath;
    Quectel_Infer _quectel_infer_class;

    int _index;
    bool _isWaitNo1;

    Bearing bear;
    Bearing_input_data bn_result_24;
    Bearing bear_4;
    Bearing_input_data bn_result_24_4;

    Bearing_neihuan_result_data bn_result_15;
    Bearing_neihuan_result_data bn_result_15_5;
    Bearing_waihuan_result_data bn_result_3;

    QMap<int, std::string> _resultTo_OKNG;
    QList<QString> _transferTo_CN;

    int _mergeSize;
    int _totalSize;
    std::vector<cv::Mat> _srcImages;
    QList<QString> _timesTrigger;

    QString _charDir;
    QMap<QString, QStringList> _charDir_all;

//    QMutex _mutex;

};









#endif // MainThread_H
