#pragma once

#include <opencv2/opencv.hpp> // ����OpenCVͷ�ļ�


class PlateLocate
{
public:
	void process(cv::Mat img);

	void rotateResult(cv::RotatedRect & rec);

private:
	bool verifySize(cv::RotatedRect &rec);
	bool verifyAngle(cv::RotatedRect &rec);
	void setRotateRecToImg(cv::Mat src, cv::RotatedRect &rec);
};