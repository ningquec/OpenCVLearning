#pragma once

#include <opencv2/opencv.hpp> // 导入OpenCV头文件


class PlateLocate
{
public:
	void process(cv::Mat img);

	cv::Mat rotateResult(cv::Mat image, cv::RotatedRect & rec);

private:
	bool verifySize(cv::RotatedRect &rec);
	bool verifyAngle(cv::RotatedRect &rec);
	void setRotateRecToImg(cv::Mat src, cv::RotatedRect &rec);
};