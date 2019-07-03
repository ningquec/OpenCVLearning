// OpenCVLearning.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <opencv2/opencv.hpp> // 导入OpenCV头文件

using namespace cv;	// 使用命名空间cv

void testSobel(Mat img)
{
	Mat sobel_x, sobel_y;
	Mat abs_sobel_x, abs_sobel_y;
	Sobel(img, sobel_x, CV_16S, 1, 0);	// x方向求导
	Sobel(img, sobel_y, CV_16S, 0, 1);	// y方向求导
	convertScaleAbs(sobel_x, abs_sobel_x);	// 将Sobel x图像转换为8位无符号整数，方便显示
	convertScaleAbs(sobel_y, abs_sobel_y);	// 将Sobel y图像转换为8位无符号整数，方便显示

	namedWindow("Sobel x");
	imshow("Sobel x", abs_sobel_x);
	namedWindow("Sobel y");
	imshow("Sobel y", abs_sobel_y);

	Mat dst;
	// 将Sobel x和Sobel y加起来，近似显示梯度
	addWeighted(abs_sobel_x, 0.5, abs_sobel_y, 0.5, 0, dst);
	namedWindow("Sobel");
	imshow("Sobel", dst);
}

void testCanny(Mat img)
{
	Mat edge;
	Canny(img, edge, 50, 150);	// 使用50，150作为双边阈值进行Canny
	namedWindow("Edge");
	imshow("Edge", edge);
}

void testLaplace(Mat img)
{
	Mat edge, abs_edge;
	Laplacian(img, edge, CV_16S, 3);	// 进行laplace变换
	convertScaleAbs(edge, abs_edge);	// 将图像转换为8bit

	namedWindow("Edge");
	imshow("Edge", abs_edge);
}

void testContour(Mat img)
{
	Mat binary;
	threshold(img, binary, 100, 255, THRESH_BINARY);	// 先进行二值化

	vector<vector<Point>> contours;		// 保存轮廓信息的数据结构
	vector<Vec4i> hierarchy;	// 保存轮廓层级信息的数据结构
	// 查找边缘
	findContours(binary, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> result_contours;	// 保存为二维码定位矩形的轮廓
	for (int i = 0; i < contours.size(); i++)
	{
		int k = i;	// 第k个边缘
		int c = 0;	// 记录子轮廓有几个
		vector<Point> vec = contours.at(i);
		// 如果有子轮廓，c+1，判断子轮廓是否仍包含轮廓
		while (hierarchy[k][2] != -1)	
		{
			k = hierarchy[k][2];
			c = c + 1;	
		}
		if (c >= 2)
		{
			Vec4i v = hierarchy[i];
			result_contours.push_back(vec);
		}
	}

	Mat result(img.size(), CV_8U, cv::Scalar(255));	// 创建一个供绘制的Mat
	drawContours(result, result_contours, -1, Scalar(0), 1);	// 绘制所有边缘
	namedWindow("Result img");
	imshow("Result img", result);
	
}

int main()
{
	Mat img = imread("../test image/qrcode.png", 0); // 以灰度图的格式读入一张图片
	namedWindow("Src image");
	imshow("Src image", img);	// 显示原图

	testSobel(img);
	testCanny(img);
	testLaplace(img);

	testContour(img);

	waitKey(0);
	return 0;
}

