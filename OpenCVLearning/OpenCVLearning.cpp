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
	threshold(img, binary, 100, 255, THRESH_BINARY);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(binary, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> result_contours;
	for (int i = 0; i < contours.size(); i++)
	{
		int k = i;
		int c = 0;
		vector<Point> vec = contours.at(i);
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
	Mat result(img.size(), CV_8U, cv::Scalar(255));
	drawContours(result, contours, -1, Scalar(0), 1);
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

