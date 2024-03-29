// OpenCVLearning.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <opencv2/opencv.hpp> // 导入OpenCV头文件

using namespace cv;	// 使用命名空间cv

int main()
{
	Mat img = imread("../test image/lena.jpg"); // 读入一张图片
	namedWindow("Lena");	// 创建一个名为 "Lena"的窗口
	imshow("Lena", img);	// 在窗口中显示图片
	waitKey(0);				// 等待键盘输入后窗口自动关闭
	return 0;
}
