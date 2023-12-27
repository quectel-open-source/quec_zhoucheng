//
// Created by quectel on 29/7/2022.
//

#ifndef SAMPLECODE_QUECTELINFER_H
#define SAMPLECODE_QUECTELINFER_H

#include <cstring>
#include <iostream>
#include <getopt.h>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include <chrono>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include <QMap>
#include <QList>
#include <QString>
#include <QDebug>


class Seg_v3_Infer;
class Seg_v1_Infer;
class Class_v1_Infer;

using namespace std;

// 算法框架类型名
typedef enum {
    FT_UNKNOWN = -1,
    FT_DET_V1 = 0,
    FT_DET_V3,
    FT_SEG_V1,
    FT_SEG_V2,
    FT_SEG_V3,
    FT_OCR,
    FT_CLASS_V1,
} QUECTEL_FRAMEWORK_TYPE;


class Quectel_Infer {
public:
    Quectel_Infer();
    ~Quectel_Infer(){        
    };

    int initial_inference(int index, string infer_config); // initial model and create input && output data buffer.
    int inference(unsigned char *srcData, unsigned char *dstData, int &predLabel, int srcWidth, int srcHeight, int nchannel, bool show,
                  int camid, bool isUseGray, std::vector<float>& times, float& sum_area);
    double back_roi_light(cv::Mat scr, std::vector<cv::Point> contour, int max_light);

    int unInit_inference();

    void setConfThresh(float confThresh);
    void setIouThresh(float iouThresh);
    void setLabels(std::vector<std::string> labels);
    void setConfJsonValue(unsigned char *pEncodedBuffer);  

    void setScoreThresh(std::vector<float> scoreThresh);
    
    void set_postParams(int camid, std::map<std::string, std::vector<int>> labels_params);
    QUECTEL_FRAMEWORK_TYPE _frameworkType;
private:

    Seg_v3_Infer *p_BuilderInfer;
 
    Seg_v1_Infer *pSeg_v1_BuilderInfer;
    Class_v1_Infer* p_BuilderInfer_Class_v1;

    std::vector<std::string> _m_labels;
    std::map<std::string, std::vector<int>> _postParams;
    int _camid;
    std::string modelPath;

};


#endif
