#include "mainThread.h"
#include <cstring>
#include <getopt.h>

#include <cstdlib>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <chrono>
#include <stdexcept>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <random>

#ifndef WIN32
#include <stdarg.h>  
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h>
#else
#include <direct.h>  
#include <io.h>  
#endif

#include <QMutex>
#include "main_zack.h"
#include <QtConcurrent>
#include <QObject>
#include <QMetaType>

using namespace std;

extern QString _cpp2Dir;
extern bool _isShowAllLabel;  // 显示每张图的label, true: 1   false: 0
extern QMap<QString, QString> _settingJsons;


void getBinary(int n)
{
    for (int i=0; i <32; i++)
    {
        int bjudge = (n >> i) & 1;
        if(bjudge == 1) {
            switch (i)
            {
            case 0:
                cout << i << " NG" << endl;
                break;
            case 1:
                cout << i << " OK" << endl;
                break;
            case 2:
                cout << i << " NULL" << endl;
                break;
            case 3:
                cout << i << " 不均匀" << endl;
                break;
            case 4:
                cout << i << " 碰伤" << endl;
                break;
            case 5:
                cout << i << " 脏污" << endl;
                break;
            case 6:
                cout << i << " 毛糙" << endl;
                break;
            case 7:
                cout << i << " 无盖" << endl;
                break;
            case 8:
                cout << i << " 凹坑" << endl;
                break;
            case 9:
                cout << i << " 划痕" << endl;
                break;
            default:
                break;
            }
        }
    }
}


MainThread::MainThread(QWidget* parent, int index, QString jsonPath, bool isAI, bool isWaitNo1, int size, int mergeSize)
{
    this->setParent(parent->parentWidget()->parentWidget());

    _resultTo_OKNG = {{0, "OK"}, {1, "NG"}, {2, "NG"}, {3, "NG"}, {4, "NG"}, {5, "NG"}};
    _transferTo_CN = {"一", "二", "三", "四", "五", "六", "七"};
    _jsonPath = jsonPath;

    _isStop = false;

    _index = index;  // 0, 1,2, 3    [4,5, 6]
    _isWaitNo1 = isWaitNo1;
    _totalSize = size;
    _mergeSize = mergeSize;

    _charDir = _cpp2Dir + "/quec_libs/characters";

    if (isAI){
        _quectel_infer = new Quectel_Infer();
    }
    else{
        QString info0 = QString("【MainThread】%1 不需要初始化").arg(_index);
        emit emit_log(info0);
        std::cout << info0.toStdString() << std::endl;
    }
}

MainThread::~MainThread() {}

void MainThread::closed()
{
    _isStop = true;
}

////////////// 0, 1, 2, 4, 5, 8
void MainThread::transform_data(ImageInfo pic_info)
{
    if (_index == 1){
        if (pic_info.campicid == 1)
            _queue.enqueue(pic_info);
    }
    else if (_index == 2){
        if (pic_info.campicid > 1){
            _queue.enqueue(pic_info);
        }
    }
    else if (_index == 5){
        if (pic_info.campicid > 1){
            _queue.enqueue(pic_info);
//            if (pic_info.campicid == 2){
//                if (pic_info.camid == 4){
//                    _srcImages_24_4.clear();
//                }
//            }
        }
    }
    else if (_index == 0){
        _queue.enqueue(pic_info);
    }
    else if (_index == 3){
        _queue.enqueue(pic_info);
    }
    printf("<-- index:%d  push: size=%d\n", _index, _queue.count());
}

///////////// 前处理
void MainThread::transform_preROI(int camid, QList<int> pre_ROI)
{
//    switch (camid)
//    {
//    case 1:{
//        bn_result_15.image_roi             = cv::Rect(pre_ROI[0], pre_ROI[1], pre_ROI[2], pre_ROI[3]);
//    }
//        break;
//    case 5:{
//        bn_result_15.image_roi             = cv::Rect(pre_ROI[0], pre_ROI[1], pre_ROI[2], pre_ROI[3]);
//    }
//        break;
//    case 4:{
//        bn_resu_36.image_roi             = cv::Rect(pre_ROI[0], pre_ROI[1], pre_ROI[2], pre_ROI[3]);
//    }
//        break;
//    }

//    qDebug() << "****** 更新前处理ROI < 相机 " << camid << " > ： " << pre_ROI;
}

void MainThread::transform_preParams(int camid, QList<int> pre_params)
{
    switch (camid)
    {
    case 1:{
        std::vector<int> values;
        for(int value: pre_params){
            values.push_back(value);
        }
        bn_result_15.setValue(values);
        bn_result_15_5.setValue(values);
    }
        break;

    case 2:{
        std::vector<int> values;
        for(int value: pre_params){
            values.push_back(value);
        }
        bn_result_24.setValue(values);
    }
        break;
    case 4:{
        std::vector<int> values;
        for(int value: pre_params){
            values.push_back(value);
        }
        bn_result_24_4.setValue(values);
    }
        break;


    case 3:{
        std::vector<int> values;
        for(int value: pre_params){
            values.push_back(value);
        }
        bn_result_3.setValue(values);
    }
        break;
    }

    qDebug() << "****** 更新前处理参数 < 相机 " << camid << " > ： " << pre_params;
}

void MainThread::transform_postParams(int camid, QMap<QString, QList<int>> labels_params)
{
    std::map<std::string, std::vector<int>> params;
    for(QString key : labels_params.keys()){
        std::vector<int> values;
        for (int va :labels_params[key]){
            values.push_back(va);
        }
        params[key.toStdString()] = values;
    }
    _quectel_infer->set_postParams(camid, params);
}


void MainThread::mainThread_input_data_received(int camid, Bearing_input_data inputData)
{
    if (camid == 2){
        bn_result_24 = inputData;
    }
    else if (camid == 4){
        bn_result_24_4 = inputData;
    }
//    if (_index == 2){
//        _isWaitNo1 = false;
//    }
}

void MainThread::mainThread_init_photo_store_received()
{
    _isWaitNo1 = false;
}


//////
void MainThread::init_inferImage(int index, Quectel_Infer* _infer)  // 0.1.2  4.5.6  8
{
    emit emit_initYes(-1, 0);
    qintptr threadId = reinterpret_cast<quintptr>(this->currentThreadId());
    QString info0 = QString("【MainThread】%1 start 初始化…   threadID=%2  json: %3").arg(index).arg(threadId).arg(_jsonPath);
    emit emit_log(info0);
    std::cout << info0.toStdString() << std::endl;

    const auto t_start1 = std::chrono::high_resolution_clock::now();
    int ret = EXIT_FAILURE;

    ret = _infer->initial_inference(index, _jsonPath.toStdString());

    if (ret == EXIT_SUCCESS){
        cv::Mat src = cv::imread("platforms/mask.jpg");
        int label = 0;
        float sum_area = 0;
        std::vector<float> times;

        _infer->inference(src.data, src.data, label, src.cols, src.rows, src.channels(), false, 1, false, times, sum_area);

        const auto t_end1 = std::chrono::high_resolution_clock::now();
        const float ms1 = std::chrono::duration<float, std::milli>(t_end1 - t_start1).count();
        emit emit_initYes(1, ms1);
        info0 = QString("【MainThread】%1 初始化完成！   threadID=%2  time=%3 ms").arg(index).arg(threadId).arg(ms1);
        emit emit_log(info0);
        std::cout << info0.toStdString() << std::endl;
    }
    else{
        emit emit_initYes(0, 0);
        info0 =  QString("【MainThread】%1 初始化失败……   threadID=%2").arg(index).arg(threadId);
        emit emit_log(info0);
        std::cout << info0.toStdString() << std::endl;

        return;
    }

}

void MainThread::init_OCR(QString text1, QString text2)
{
    QDir q_dir(_charDir);
    if (q_dir.exists() && _charDir_all.isEmpty()){
        QStringList dirNames = q_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        foreach(QString dirname, dirNames)
        {
            QDir file_dir(_charDir + "/" + dirname);
            file_dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
            QFileInfoList fileList = file_dir.entryInfoList();

            QList<QString> paths;
            foreach(QFileInfo file, fileList)
            {
                paths.append(file.filePath());
            }
            _charDir_all[dirname.toUpper()] = paths;
        }
        emit emit_initChar(_index, _charDir_all.keys());
    }
    qDebug() << "11111111111111111111111     " << _index << "    " << _charDir_all.keys();

    QList<QString> paths_cur;
    std::vector<std::vector<cv::Mat>> image_total;
    QList<QString> keys = _charDir_all.keys();

    QString text_1 = text1.toUpper();
    bool yes_1 = false;
    if (keys.contains(text_1)){
        paths_cur = _charDir_all[text_1];

        std::vector<cv::Mat> images1;
        foreach(QString path, paths_cur)
        {
            images1.push_back(cv::imread(path.toStdString(), 0));
        }
        image_total.push_back(images1);
        yes_1 = true;
    }

    QString text_2 = text2.toUpper();
    bool yes_2 = false;
    if (keys.contains(text_2)){
        paths_cur = _charDir_all[text_2];

        std::vector<cv::Mat> images2;
        foreach(QString path2, paths_cur)
        {
            images2.push_back(cv::imread(path2.toStdString(), 0));
        }
        image_total.push_back(images2);
        yes_2 = true;
    }

    if (yes_1){
        qDebug() << ">>>>>>>>>>>>>>>>  load Char: " << text1;
    }
    if (yes_2){
        qDebug() << ">>>>>>>>>>>>>>>>  load Char: " << text2;
    }


    bear.init(image_total);
    bear_4.init(image_total);

}


///////////
void MainThread::run()
{
//    if (_index == 0){
//        _quectel_infer_class = Quectel_Infer();

//        QString path = _jsonPath;
//        path.replace("1.json", "11.json");
//        _quectel_infer_class.initial_inference(-1, path.toStdString());
//    }
//    else
    if (_index == 1){
        this->init_OCR();
    }


    this->init_inferImage(_index, _quectel_infer);

    while(!_isStop)
    {
        if (_queue.isEmpty()/* || _isWaitNo1*/){
            QThread::msleep(10);
            continue;
        }
//        if (_index == 3)
//            _isWaitNo1 = true;

        QDateTime t_start1 = QDateTime::currentDateTime();
        ImageInfo img_info = _queue.dequeue();
//        printf("\n--> pop: size=%d\n", _queue.count());

        QString info = QString("----> SN=%1, camid=%2, campicid=%3  threadID: %4  inference...   %5")
                .arg(img_info.SN).arg(img_info.camid).arg(img_info.campicid)
                .arg(QString::number(reinterpret_cast<quintptr>(this->currentThreadId())))
                .arg(t_start1.toString("yyyy-MM-dd  hh:mm:ss.zzz"));
        printf("%s\n", info.toLatin1().data());
        emit emit_log(info);

        ///////////////////////////////////// 1. inference....///////////////////////////
        int predLabel = 0;
        bool isShow = true;
        QDateTime t_start2 = QDateTime::currentDateTime();

//        cv::Mat srcImage = img_info.srcImage;
        cv::Mat srcImage = cv::Mat(img_info.height, img_info.width, CV_8UC1, img_info.srcData);
        cv::Mat dstImage;
        float sum_area = 0;
        // 推理
        this->inference(srcImage, dstImage, predLabel, isShow, img_info, sum_area);

        const float ms2 = t_start2.msecsTo(QDateTime::currentDateTime());
        info = QString("----> SN=%1, camid=%2, campicid=%3  threadID: %4  inference all: %5 ms   %6")
                .arg(img_info.SN).arg(img_info.camid).arg(img_info.campicid)
                .arg(QString::number(reinterpret_cast<quintptr>(this->currentThreadId()))).arg(ms2)
                .arg(t_start1.toString("yyyy-MM-dd  hh:mm:ss.zzz"));
        emit emit_log(info);
        printf("%s\n", info.toLatin1().data());

        ///////////////////////////////////// 2. send info ///////////////////////////////
//        std::random_device rd;
//        std::mt19937 gen(rd());
//        std::uniform_int_distribution<int> dist(0, 1);
//        predLabel = dist(gen);

//        emit emit_results(Mat2QImage(dstImage), predLabel, img_info.merge_path, img_info.campicid, img_info.time_start, sum_area);

        bool isMergeResult = false;
        isMergeResult = merge(img_info, dstImage, predLabel, sum_area);


        ///////////////////////////////////// 3. write info ///////////////////////////////

        if (dstImage.cols > 0 && _saveImgsNums >= 0){
            QString dirname_last = "相机" + this->_transferTo_CN[img_info.camid - 1];

            QString path_src = img_info.src_path;
            path_src = path_src.replace(dirname_last, dirname_last + "/" + QString::fromStdString(this->_resultTo_OKNG[predLabel]));
            cv::imwrite(path_src.toStdString(), srcImage);

            QString path_dst = img_info.dst_path;
            path_dst = path_dst.replace(dirname_last, dirname_last + "/" + QString::fromStdString(this->_resultTo_OKNG[predLabel]));
            path_dst = path_dst.replace("0_org", QString::number(predLabel) + "_res");
            cv::imwrite(path_dst.toStdString(), dstImage);
        }

        printf("#########  %d  %d  yes!\n", img_info.camid, img_info.campicid);
        free(img_info.srcData);

        if (isMergeResult){
            QDateTime end = QDateTime::currentDateTime();
            QString info3 = QString("#### 相机 %1  start: %2  end: %3  总运行时间：%4 ms").arg(img_info.camid)
                    .arg(img_info.time_start.toString("yyyy-MM-dd hh:mm:ss.zzz"))
                    .arg(end.toString("yyyy-MM-dd hh:mm:ss.zzz"))
                    .arg(img_info.time_start.msecsTo(end));

            emit emit_log(info3);
        }

    }

    _quectel_infer->unInit_inference();
//    if (_index == 0)
//        _quectel_infer_class.unInit_inference();
}


/////// 1.
void MainThread::inference(Mat& srcImage, Mat& dstImage, int& predLabel, bool isShow, ImageInfo img_info, float& sum_area)
{
//    printf("  start inference…     %d  %d\n", img_info.camid, img_info.campicid);
    QDateTime time_start = QDateTime::currentDateTime();

    bool isStartAI = false;
    int result_1 = 0;
    int result_1_4 = 0;
    int result_2 = 0;
    bool isUseGray = false;
    cv::Mat srcImage2 = srcImage.clone();
    cv::Mat aiImage;

    // 1.前处理
    switch(img_info.camid)
    {
    case 2:
        if (img_info.campicid == 1){
            result_1 = 1;
            result_1 =  main25_DuanMian(srcImage2, bear, bn_result_24);

            emit emit_input_data(img_info.camid, bn_result_24);
            isUseGray = true;
            srcImage2 = srcImage2(bn_result_24.bear_info.cut_roi).clone();

            result_1 = main25_DuanMian_1(srcImage2, dstImage, aiImage, bear, bn_result_24);
        }
        else{
            result_1 = 1;
            cv::Mat gray = srcImage2(bn_result_24.bear_info.cut_roi).clone();
            if (gray.channels() == 3)
                cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);
            _srcImages.push_back(gray.clone());
            _timesTrigger.append(img_info.time_start.toString("hhmmss"));

            if (_srcImages.size() == 4 && img_info.campicid == 5){
                std::vector<cv::Mat> images_cur;
                for(cv::Mat img: _srcImages){
                    images_cur.push_back(img);
                }
                _srcImages.clear();
                _timesTrigger.clear();

                isStartAI = true;
                result_1 = main25_DuanMian_2(images_cur, aiImage, bear, bn_result_24);
                dstImage = aiImage.clone();

                if (result_1 != 0){
                    isStartAI = false;
                }
            }
            else if (_srcImages.size() > 4){
                QMap<QString, QList<int>> src_times_idxs;
                std::vector<cv::Mat> src_images_new;

                for(int i=0; i<_timesTrigger.count(); i++)
                {
                    QString time = _timesTrigger[i];

                    if (src_times_idxs.keys().contains(time)){
                        src_times_idxs[time].append(i);
                    }
                    else{
                        src_times_idxs[time] = {i};
                    }
                }

                for(QString tim: src_times_idxs.keys())
                {
                    if (src_times_idxs[tim].count() == 4){
                        for(int idx: src_times_idxs[tim]){
                            src_images_new.push_back(_srcImages[idx].clone());
                        }
                        break;
                    }
                }

                if (src_images_new.size() == 4){
                    isStartAI = true;

                    result_1 = main25_DuanMian_2(src_images_new, aiImage, bear, bn_result_24);
                    dstImage = aiImage.clone();
                    _srcImages.clear();
                    _timesTrigger.clear();

                    if (result_1 != 0){
                        isStartAI = false;
                    }
                }
            }
        }
        break;
    case 4:
        if (img_info.campicid == 1){
            result_1_4 = 1;
            result_1_4 =  main25_DuanMian(srcImage2, bear_4, bn_result_24_4);
//            std::cout << "--> 1  max_side_circle_r: " << bn_result_24_4.max_side_circle_r << std::endl;
//            std::cout << "--> 1  min_side_circle_r: " << bn_result_24_4.min_side_circle_r << std::endl;

            emit emit_input_data(img_info.camid, bn_result_24_4);
            isUseGray = true;
            srcImage2 = srcImage2(bn_result_24_4.bear_info.cut_roi).clone();

            result_1_4 = main25_DuanMian_1(srcImage2, dstImage, aiImage, bear_4, bn_result_24_4);

        }
        else{
            result_1 = 1;
            cv::Mat gray = srcImage2(bn_result_24_4.bear_info.cut_roi).clone();
            if (gray.channels() == 3)
                cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);
            _srcImages.push_back(gray.clone());
            _timesTrigger.append(img_info.time_start.toString("hhmmss"));

            if (_srcImages.size() == 4 && img_info.campicid == 5){
                std::vector<cv::Mat> images_cur;
                for(cv::Mat img: _srcImages){
                    images_cur.push_back(img);
                }
                _srcImages.clear();
                _timesTrigger.clear();

                result_1 = main25_DuanMian_2_4(images_cur, aiImage, bear_4, bn_result_24_4);
                dstImage = aiImage.clone();

                isStartAI = true;
                if (result_1 != 0){
                    isStartAI = false;
                }
            }
            else if (_srcImages.size() > 4){
                QMap<QString, QList<int>> src_times_idxs;
                std::vector<cv::Mat> src_images_new;

                for(int i=0; i<_timesTrigger.count(); i++)
                {
                    QString time = _timesTrigger[i];

                    if (src_times_idxs.keys().contains(time)){
                        src_times_idxs[time].append(i);
                    }
                    else{
                        src_times_idxs[time] = {i};
                    }
                }

                for(QString tim: src_times_idxs.keys())
                {
                    if (src_times_idxs[tim].count() == 4){
                        for(int idx: src_times_idxs[tim]){
                            src_images_new.push_back(_srcImages[idx].clone());
                        }
                        break;
                    }
                }

                if (src_images_new.size() == 4){
                    isStartAI = true;

                    result_1 = main25_DuanMian_2(src_images_new, aiImage, bear_4, bn_result_24_4);
                    dstImage = aiImage.clone();
                    _srcImages.clear();
                    _timesTrigger.clear();

                    if (result_1 != 0){
                        isStartAI = false;
                    }
                }
            }
        }
         break;
    case 1:
        result_1 = 1;
        result_1 = main14_NeiKui(srcImage2, aiImage, bear, bn_result_15);

        dstImage = aiImage.clone();
        isStartAI = result_1 != 0 ? false : true;

        break;
    case 5:
        result_1_4 = 1;
        result_1_4 = main14_NeiKui(srcImage2, aiImage, bear_4, bn_result_15_5);

        dstImage = aiImage.clone();
        isStartAI = result_1_4 != 0 ? false : true;

        break;
    case 3:
        result_1 = 1;
        result_1 = main3_WaiHuanMian(srcImage2, aiImage, bear, bn_result_3);

        dstImage = aiImage.clone();
        isStartAI = result_1 != 0 ? false : true;

        break;
    }

    srcImage = aiImage.clone();
    QString info = QString("----> SN=%1, camid=%2, campicid=%3, threadID: %4, end algo_Pre: %5 ms")
            .arg(img_info.SN).arg(img_info.camid).arg(img_info.campicid)
            .arg(QString::number(reinterpret_cast<quintptr>(this->currentThreadId())))
            .arg(time_start.msecsTo(QDateTime::currentDateTime()));
    emit emit_log(info);
    printf("%s\n", info.toLatin1().data());

    // 2. 开启 Ai inference
    if (isStartAI){
        cv::Mat srcImage3 = aiImage.clone();
        cv::Mat dstImage2 = dstImage.clone();
        if (srcImage3.channels() == 1)
            cv::cvtColor(srcImage3, srcImage3, cv::COLOR_GRAY2BGR);
        if (dstImage2.channels() == 1)
            cv::cvtColor(dstImage2, dstImage2, cv::COLOR_GRAY2BGR);

        int result_3 = 0;
//        if (_index == 0){
//            std::vector<float> infer_times_1;
//            float area_1 = 0;
//            _quectel_infer_class.inference(srcImage3.data, dstImage2.data, result_3, srcImage3.cols, srcImage3.rows, srcImage3.channels(), isShow,
//                                           img_info.camid, isUseGray, infer_times_1, area_1);
//            srcImage3 = dstImage2.clone();
//            info = QString("----> SN=%1, camid=%2, campicid=%3, threadID: %4, end algo_Class: %5 ms")
//                        .arg(img_info.SN).arg(img_info.camid).arg(img_info.campicid)
//                    .arg(QString::number(reinterpret_cast<quintptr>(this->currentThreadId()))).arg(infer_times_1[0]);
//            emit emit_log(info);
//            printf("%s\n", info.toLatin1().data());
//        }

        std::vector<float> infer_times;
        float area = 0;
        _quectel_infer->inference(srcImage3.data, dstImage2.data, result_2, srcImage3.cols, srcImage3.rows, srcImage3.channels(), isShow,
                                  img_info.camid, isUseGray, infer_times, area);
        dstImage = dstImage2.clone();
        sum_area = area;
        result_2 = (result_3 == 0 && result_2 == 0) ? 0 : 1;

        info = QString("----> SN=%1, camid=%2, campicid=%3, threadID: %4, end algo_Infer: %5 ms, end algo_Post: %6 ms")
                    .arg(img_info.SN).arg(img_info.camid).arg(img_info.campicid)
                .arg(QString::number(reinterpret_cast<quintptr>(this->currentThreadId()))).arg(infer_times[0]).arg(infer_times[1]);
        emit emit_log(info);
        printf("%s\n", info.toLatin1().data());
    }

    predLabel = (result_1 == 0 && result_1_4 == 0 && result_2 == 0) ? 0 : 1;
    if (img_info.camid == 1 && result_1 != 0){
        predLabel = 2;
    }
    else if (img_info.camid == 5 && result_1_4 != 0){
        predLabel = 2;
    }
}

/////// 2.
bool MainThread::merge(ImageInfo img_info, cv::Mat dst, int predLabel, float sum_area)
{
    bool isSendResult = false;

    if (img_info.camid == 2){
        if (img_info.campicid == 1){
            merge_func(img_info, dst, predLabel, sum_area);
        }
        else if(img_info.campicid == 5){
            isSendResult = merge_func(img_info, dst, predLabel, sum_area);
        }
    }
    if (img_info.camid == 4){
        if (img_info.campicid == 1){
            merge_func(img_info, dst, predLabel, sum_area);
        }
        else if(img_info.campicid == 5){
            isSendResult = merge_func(img_info, dst, predLabel, sum_area);
        }
    }

    if (img_info.camid == 3){
        isSendResult = merge_func(img_info, dst, predLabel, sum_area);
    }

    if (img_info.camid == 1){
        isSendResult = merge_func(img_info, dst, predLabel, sum_area);
    }
    if (img_info.camid == 5){
        isSendResult = merge_func(img_info, dst, predLabel, sum_area);
    }

    return isSendResult;
}

bool MainThread::merge_func(ImageInfo img_info, cv::Mat dst, int predLabel, float sum_area)
{
    bool isSendResult = false;
    QImage dst_qimage = Mat2QImage(dst);

    QString path = img_info.merge_path;
    path = path.replace("_0_org", QString("_%1_%2_merge").arg(img_info.camid).arg(predLabel));

    QImage mergeImg0 = this->drawImages_qimage(img_info.camid, dst_qimage.copy(), 3, predLabel);
    emit emit_results(img_info.camid, img_info.campicid, mergeImg0, predLabel, img_info.SN, sum_area);

    if (_saveImgsNums == 0)
        mergeImg0.save(path, "JPG");
    isSendResult = true;
    this->generate_mergeNG(mergeImg0.copy(), predLabel, path);

    return isSendResult;
}

QImage MainThread::MergeImages_qimage(QList<QImage> images, int rows, int cols, int row_spacing, int col_spacing, int channels, int result, bool showLabel, QList<int> labels)
{
    int w0 = images[0].width();
    int h0 = images[0].height() ;
    int total_width = w0 * cols + col_spacing * (cols - 1);
    int total_height = h0* rows + row_spacing * (rows - 1);

    QImage mergeImage;
    if (channels == 3)
        mergeImage = QImage(total_width, total_height, QImage::Format_RGB888);
    else
        mergeImage = QImage(total_width, total_height, QImage::Format_Grayscale8);
    mergeImage.fill(Qt::black);

   QPainter painter(&mergeImage);
   painter.setRenderHint(QPainter::Antialiasing);

   int y_offset = 0;
   int cnts = images.size();
   for (int r = 0; r < rows; ++r) {
       int x_offset = 0;
       for (int c = 0; c < cols; ++c) {
           int image_index = r * cols + c;

           if (image_index < cnts) {
               QRect roi;
               if (images[image_index].width() >= w0)
                   roi = QRect(x_offset, y_offset, images[image_index].width(), images[image_index].height());
               else
                   roi = QRect(x_offset + (w0 - images[image_index].width())/2, y_offset + (h0 - images[image_index].height())/2, images[image_index].width(), images[image_index].height());

               painter.drawImage(roi, images[image_index]);
//               images[image_index].save(QString::number(_index) + "__" + x_offset + "_" + y_offset + ".jpg", "JPG");

               if (showLabel){
                   QFont font1;
                   int size1 = images[image_index].width() >= images[image_index].height() ? images[image_index].height() / 10 : images[image_index].width() / 10;
                   font1.setPointSize(size1);

                   QString text1;
                   QColor color1;
                   if (labels[image_index] == 0){
                       text1 = "OK";
                       color1 = QColor(0, 255, 0);
                   }
                   else{
                       text1 = "NG";
                       color1 = QColor(255, 0, 0);
                   }
                   painter.setFont(font1);
                   painter.setPen(color1);
                   painter.drawText(roi.x() + 10, roi.y() + 10+size1, text1);
               }

           }
           x_offset += images[0].width() + col_spacing;
       }
       y_offset += images[0].height() + row_spacing;
   }

   QFont font;
   int size = total_width >= total_height ? total_height / 10 : total_width / 10;
   font.setPointSize(size);
   font.setBold(true);

   QString text;
   QColor color;
   if (result == 0){
       text = "OK";
       color = QColor(0, 255, 0);
   }
   else{
       text = "NG";
       color = QColor(255, 0, 0);
   }
   painter.setFont(font);
   painter.setPen(color);
   painter.drawText(10, 10+size, text);

   return mergeImage;
}

QImage MainThread::drawImages_qimage(int camid, QImage images, int channels, int result)
{
    QImage tmp = images.copy();
    int w = tmp.width();
    int h = tmp.height();

    int font_size = w >= h ? h / 10 : w / 10;
    QImage img_1, img_2;
    if (camid == 3){
        QMatrix matrix;
        matrix.rotate(90);
        QImage tmp2 = tmp.transformed(matrix);

        w = tmp2.width() / 2;
        h = tmp2.height() * 2;
        font_size = h / 10;

        img_1 = tmp2.copy(0, 0, tmp2.width() / 2, tmp2.height()).copy();
        img_2 = tmp2.copy(tmp2.width() / 2, 0, tmp2.width() / 2, tmp2.height()).copy();
    }
    else{
        img_1 = tmp.copy();
    }

    QImage mergeImage;
    if (channels == 3)
        mergeImage = QImage(w, h, QImage::Format_RGB888);
    else
        mergeImage = QImage(w, h, QImage::Format_Grayscale8);
    mergeImage.fill(Qt::black);

   QPainter painter(&mergeImage);
   painter.setRenderHint(QPainter::Antialiasing);

   if (camid == 3){
       painter.drawImage(0, 0, img_1);
       painter.drawImage(0, h / 2, img_2);
   }
   else{
       painter.drawImage(0, 0, img_1);
   }

   QFont font;
   font.setPointSize(font_size);
   font.setBold(true);

   QString text;
   QColor color;
   if (result == 0){
       text = "OK";
       color = QColor(0, 255, 0);
   }
   else{
       text = "NG";
       color = QColor(255, 0, 0);
   }
   painter.setFont(font);
   painter.setPen(color);
   painter.drawText(10, 10+font_size, text);
   painter.end();

   return mergeImage;
}

void MainThread::generate_mergeNG(QImage mergeImg, int result, QString path)
{
    if (result != 0){
        QString path_ng = path;
        path_ng = path_ng.replace("略图", "略图-NG");

//        QFile::copy(path, path_ng);
        if (_saveImgsNums >= 0)
            mergeImg.save(path_ng, "JPG");
    }
}

QImage MainThread::drawChineseText(const QImage& q_image, QList<QString> texts, QList<QList<int>> pts, QList<int> fontSizes, QList<QColor> colors)
{
    QPixmap pixmap = QPixmap::fromImage(q_image);
    QPainter painter(&pixmap);
    for (int i=0; i<texts.count(); i++)
    {
        QFont font("微软雅黑", fontSizes[i]);  // SimSun  SimHei
        painter.setFont(font);
        painter.setPen(colors[i]);
        painter.drawText(pts[i][0], pts[i][1], texts[i]);
    }
    painter.end();

    return pixmap.toImage();
}

