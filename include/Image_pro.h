#pragma once
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class CircleFit : public cv::MinProblemSolver::Function
{
public:
	CircleFit(const std::vector<cv::Point>& points) : m_points(points) {}

	int getDims() const override
	{
		return 3; // We have three unknowns (x, y, r)
	}

	double calc(const double* x) const override
	{
		double res = 0;
		cv::Point2f center(x[0], x[1]);
		float r = x[2];
		for (const auto& point : m_points)
		{
			double d = cv::norm(cv::Point2f(point.x, point.y) - center) - r;
			res += d * d;
		}
		return res;
	}

private:
	const std::vector<cv::Point>& m_points;
};
struct Circle
{
	cv::Point2f center;
	float radius;
};
class Image_pro {
public:
	Image_pro(void);
	int get_circle(vector<Point> vp, cv::Point& pt);
	vector<Point> get_max_area_contore(cv::Mat image,float max_c);
	vector<Mat> photo_store(std::vector<cv::Mat>camImages);
	vector<Mat> photo_store1(std::vector<cv::Mat>camImages);
    Mat CircleToRectangle(const Mat& circle, const Point& Center, int Radius, int RingStride, double angle);
	float cuacul_r(vector<Point> contour, Point2f center, int side_dis,int in_dis, vector<int>& data_return, int index);
	float cuacul_any(vector<Point> contour, Point2f center, int side_dis, int in_dis, vector<int>& data_return, int index);
	void HistSpecify(const Mat& src_gray, const Mat& ref_gray, Mat& result);
	vector<vector<Point>> pointtocircle(vector<vector<Point>> vps, const Point2f& Center, int Radius);
	vector<vector<Point>> deal_side_1(cv::Mat cricle_side, int expad_side, int threshold_bin , int min_area );
	vector<vector<int>> get_side_bound(cv::Mat bin_image);
	int mode(const std::vector<int>& data);
	Circle fitCircle(const std::vector<cv::Point>& points);
	vector<Point> find_maoci(cv::Mat gray, float max_side, float min_side);
private:
};

