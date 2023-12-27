#ifndef MAIN_ZACK_H
#define MAIN_ZACK_H

#include "bearing.h"


/////////////////  端面
int main25_DuanMian(cv::Mat src, Bearing& bear, Bearing_input_data& input_data)
{
    //可以设置一个roi进行提速
    input_data.bear_info=	bear.get_image_bear_info(src, "S007");

    if (input_data.bear_info.circles.size() != 4)
    {
        std::cout << "异常: size=" << input_data.bear_info.circles.size() << std::endl;
        return 1;
    }

    return 0;

}

int main25_DuanMian_1(cv::Mat src, cv::Mat& dst, cv::Mat& ai, Bearing& bear, Bearing_input_data& input_data)
{
    auto start = std::chrono::high_resolution_clock::now();

    Bearing_result_data result_data;

    result_data = bear.deal_thrid_image(src, ai, input_data);//第一个工位进行处理,亮边处理凹坑和相关黑色区域内容

    dst = result_data.roi_image.clone();

    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start);
//    std::cout << "  ## 端面 1  时间： " << duration1.count() << " 毫秒  " << dst.size() << std::endl;

    return result_data.current_bear_type;
}

int main25_DuanMian_2(vector<cv::Mat> images, cv::Mat& ai, Bearing& bear, Bearing_input_data& input_data)
{
    //四张光度立体后进处理
    auto start = std::chrono::high_resolution_clock::now();

    Bearing_result_data result_data4 = bear.deal_four_image(images, input_data, 1);
    ai = result_data4.roi_image.clone();

//    std::cout << "  current_bear_type=" <<result_data4.current_bear_type <<  std::endl;

    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start);
//    std::cout << "  ## 端面2 -- 2,3,4,5  时间: " << duration1.count() << " 毫秒" << std::endl;

    return result_data4.current_bear_type;
}
int main25_DuanMian_2_4(vector<cv::Mat> images, cv::Mat& ai, Bearing& bear, Bearing_input_data& input_data)
{
    //四张光度立体后进处理
    auto start = std::chrono::high_resolution_clock::now();

    Bearing_result_data result_data4 = bear.deal_four_image(images, input_data, 1);
    ai = result_data4.roi_image.clone();

//    std::cout << "  current_bear_type=" <<result_data4.current_bear_type <<  std::endl;

    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start);
//    std::cout << "  ## 端面4 -- 2,3,4,5  时间: " << duration1.count() << " 毫秒" << std::endl;

    return result_data4.current_bear_type;
}



/////////////////  内环面 + 内倒角
int main14_NeiKui(cv::Mat src, cv::Mat& dst, Bearing& bear, Bearing_neihuan_result_data& input_data)
{
    auto start = std::chrono::high_resolution_clock::now();
    Mat srcImage = src.clone();

    cv::Mat cnn_, Intern_L;
    Bearing_neihuan_result_data return_data = bear.get_neikui(src, cnn_, Intern_L, input_data);
    if (return_data.current_bear_type == return_data.OK)
    {
        //一定有图片
        //cnn 进行分类 intern进行模型处理
        //cnn 是ng的话 绘制一个圆形   模型锈斑或者划痕绘制缺陷显示intern_l缺陷
        dst = Intern_L.clone();
    }
    else
    {
        //直接显示图像src ng信号
        dst = srcImage.clone();
    }

    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start);
//    std::cout << "  ## 内环  时间： " << duration1.count() << " 毫秒  " << dst.size() << std::endl;

    return return_data.current_bear_type;
}



/////////////////  外环面 + 外倒角
int main3_WaiHuanMian(cv::Mat src, cv::Mat& dst, Bearing& bear, Bearing_waihuan_result_data& input_data)
{
    auto start = std::chrono::high_resolution_clock::now();

    cv::Mat crop_roi = src.clone(); // src(input_data.image_roi);

    cv::Mat AI_image;
    Bearing_waihuan_result_data result_data = bear.xianshao(src, AI_image,input_data);

    dst = result_data.roi_image.clone();

    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start);
//    std::cout << "  ## 外环  时间： " << duration1.count() << " 毫秒  " << dst.size() << std::endl;

    return result_data.current_bear_type;

}




#endif // MAIN_ZACK_H
