// OpenCVLearning.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <opencv2/opencv.hpp> // 导入OpenCV头文件

using namespace cv;	// 使用命名空间cv

void testLaplace(Mat src)
{
	Mat down_img;
	pyrDown(src, down_img);	// 降采样
	namedWindow("Down image");
	imshow("Down image", down_img);

	Mat up_img;
	pyrUp(down_img, up_img);	// 向上采样
	namedWindow("Up image");
	imshow("Up image", up_img);

	Mat laplace = src - up_img;	// 拉普拉斯 = 原图 - 向上采样（降采样（原图））
	namedWindow("Laplace image");
	imshow("Laplace image", laplace);

	waitKey(0);
}

void testResize(Mat src)
{
	Mat zoom_out;
	resize(src, zoom_out, Size(200, 200));	// 直接指定输出尺寸
	namedWindow("Zoom out image");
	imshow("Zoom out image", zoom_out);

	Mat zoom_in;
	resize(zoom_out, zoom_in, Size(0, 0), 2, 2);	// 直接指定放缩因子
	namedWindow("Zoom in image");
	imshow("Zoom in image", zoom_in);

	waitKey(0);
}

int main()
{
	Mat img = imread("../test image/lena.jpg"); // 读入一张图片
	namedWindow("Src image");
	imshow("Src image", img);	// 显示原图

	testLaplace(img);	// 测试拉普拉斯
	
	testResize(img);	// 测试resize函数

	return 0;
}

