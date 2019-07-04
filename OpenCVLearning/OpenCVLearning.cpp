// OpenCVLearning.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp> // 导入OpenCV头文件

using namespace cv;	// 使用命名空间cv

int main()
{
	Mat image = imread("../test image/lena.jpg");
	namedWindow("src");
	imshow("src", image);

	Mat rotate_mat = getRotationMatrix2D(		// 获取旋转矩阵
		Point(image.rows / 2, image.cols / 2), 
		30, 1);

	rotate_mat.at<double>(0, 2) = 50; //水平平移量
	rotate_mat.at<double>(1, 2) = 50; //竖直平移量

	Mat result;
	warpAffine(image, result, rotate_mat, image.size());// 仿射变换
	namedWindow("result");
	imshow("result", result);

	waitKey(0);
	return 0;
}

