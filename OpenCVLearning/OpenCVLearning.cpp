// OpenCVLearning.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <opencv2/opencv.hpp> // 导入OpenCV头文件

using namespace cv;	// 使用命名空间cv


int main()
{
	Mat img = imread("../test image/lena.jpg"); // 以灰度图的格式读入一张图片
	namedWindow("Src image");
	imshow("Src image", img);	// 显示原图

	Mat gray_img;
	cvtColor(img, gray_img, COLOR_BGR2GRAY);
	Mat templ = imread("../test image/template.bmp", 0);
	namedWindow("Template image");
	imshow("Template image", templ);	// 显示模板图像

	Mat result;
	matchTemplate(gray_img, templ, result, CV_TM_CCOEFF_NORMED);	// 匹配模板图像

	double max_val; Point max_loc;
	minMaxLoc(result, NULL, &max_val, NULL, &max_loc, Mat());	// 找到结果图像最大值位置
	rectangle(img, max_loc,
		Point(max_loc.x + templ.cols, max_loc.y + templ.rows), 
		Scalar::all(0)); // 在图上绘制搜索到的位置

	namedWindow("Result image");
	imshow("Result image", img);	// 显示原图
	waitKey(0);
	return 0;
}

