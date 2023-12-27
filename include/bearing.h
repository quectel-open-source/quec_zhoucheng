#pragma once
#include <cstring>

#include <cstdlib>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <chrono>
#include <map>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <string>
#include "Image_pro.h"
using namespace std;
using namespace cv;
struct Bearing_result_data
{
    Rect image_roi;
    enum Bearing {
        OK=0,
        NG_detect = 1,
        NG_burr = 2,
        NG_radius = 3,
        NG_Inner = 4,
        NG_Outer = 5,
    };
    int type = 1;
    vector<vector<Point>> ng_in_out_points = vector < vector<Point>>();
    vector<vector<Point>> ng_side_face = vector < vector<Point>>();
    vector<vector<Point>> ng_in_face = vector < vector<Point>>();
    vector<float> ng_circle = vector<float>();
    //cv::Mat draw_image = Mat();
    cv::Mat roi_image = Mat();
    Bearing current_bear_type;
    Point2f cent_point = Point2f();
    vector<float> rs = vector<float>();
    vector<Rect> duanmian_rect = vector<Rect>();
    int save_index = 0;
};


struct Bearing_neihuan_result_data
{

    enum Bearing {
        OK = 0,
        NG_detect = 1,
        NG_burr = 2,
        NG_radius = 3,
        NG_Inner = 4,
        NG_Outer = 5,
    };
    int type = 1;
    Bearing current_bear_type;

    void setValue(std::vector<int> values)
    {

    }
};

struct Bearing_waihuan_result_data
{
//    Rect image_roi = cv::Rect(300, 750, 1880, 850);

    enum Bearing {
        OK = 0,
        NG_detect = 1,
        NG_burr = 2,
        NG_radius = 3,
        NG_Inner = 4,
        NG_Outer = 5,
    };
    int left_width = 12; //线扫导角宽度
    int right_width = 12; //线扫导角宽度
    int left_in = 0;
    int left_out = 0;
    int right_in = 0;
    int right_out = 0;
    int number = 3;//连续凹陷个数
    float width_min = 0.0;//凹陷程度
    int left_side = 74;//左边切图宽度64 74
    int right_side = 467;//右边切图宽度474 477

    cv::Mat roi_image = Mat();
    Bearing current_bear_type;


    void setValue(std::vector<int> values)
    {
        left_width =  values[0]; //线扫导角宽度
        right_width = values[1]; //线扫导角宽度
        number =      values[2];//连续凹陷个数
        width_min =   values[3];//凹陷程度
        left_side =   values[4];//左边切图宽度64 74
        right_side =  values[5];
    }

};


struct Bearing_type_info
{
    string bearing_name;//??
    int Type = 0;//0???  1???
    //?????;
    Point center_point;//???
    //????;
    Size Image_size;//??
    vector<float> circles= vector<float>();//????
    cv::Rect cut_roi;


};
struct Bearing_input_data
{
    //first_image param

    //int binary_threshold = 200;//?????
    //int min_area_value = 10;//??????
    int max_side_circle_r = 45;//最大外环公差
    int min_side_circle_r = 15;//最小外环公差
    int max_in_circle_r = 45;//最大内环公差
    int min_in_circle_r = 15;//最小内环公差

    //thr
    int min_wid_neijing = 10; //内径宽度
    int min_light_neijing = 160;//内径明暗度
    int min_areaa_neijing = 2000;//内径面积

    int one_area = 40;  //严重允许面积
    int two_area = 60;//轻微允许面积
    int thr_area = 100;//次级允许面积
    int one_light = 100;//严重黑块
    int two_light = 150;//轻微黑块
    int thr_light = 170;//次级黑块
    int expand_side = -5;//凹陷距离

    Bearing_type_info bear_info;//???? ???

    void setValue(std::vector<int> values)
    {
        max_side_circle_r =  values[0];
        min_side_circle_r = values[1];
        max_in_circle_r =      values[2];
        min_in_circle_r =   values[3];

        min_wid_neijing =   values[4];
        min_light_neijing =  values[5];
        min_areaa_neijing =  values[6];

        one_area =   values[7];
        two_area =  values[8];
        thr_area = values[9];
        one_light =  values[10];
        two_light = values[11];
        thr_light =      values[12];

        expand_side = values[13];
    }
};


class Bearing {
public:
    bool deal_ocr(cv::Mat image, vector<vector<Point>>& vp_ocr, int type);
    Bearing_waihuan_result_data xianshao(cv::Mat src, cv::Mat& AI, Bearing_waihuan_result_data input_data);
    Bearing_result_data deal_thrid_image(cv::Mat src, cv::Mat& AI, Bearing_input_data input_data);
    Bearing_result_data deal_four_image(vector<cv::Mat> src, Bearing_input_data input_data, float k_size = 0.5f);
    //Bearing_result_data find_ocr_location(cv::Mat, int type);
    int deal_image_data(cv::Mat image);
    Mat get_deal_circle_image(cv::Mat src, Bearing_input_data input_data);
    Bearing_neihuan_result_data get_neikui(cv::Mat src, cv::Mat& cnn_src, cv::Mat& AOI, Bearing_neihuan_result_data input_data);
    Bearing_type_info get_image_bear_info(cv::Mat src,string bear_name);
    bool write_bear_info(string json_path);
    Bearing_type_info read_bear_info(string json_path);
    Mat draw_result(cv::Mat src, Bearing_result_data bd);
    int index_111 = 110;
    cv::HOGDescriptor hog = HOGDescriptor(Size(256, 32), Size(8, 8), Size(4, 4), Size(4, 4), 3);
    void init(vector<vector<Mat>> images);
    int ocr_w = 1;
    vector< std::vector<float> >festures;
private:
    Image_pro image_pro;
    int sum_index_current = 0;
};
