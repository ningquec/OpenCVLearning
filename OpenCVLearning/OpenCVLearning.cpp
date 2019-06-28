// OpenCVLearning.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <opencv2/opencv.hpp> // 导入OpenCV头文件

using namespace cv;	// 使用命名空间cv

Mat testBoxBlur(Mat src, Size ksize, bool normalize)
{
	Mat out;
	boxFilter(src, out, -1, ksize, Point(-1, -1), normalize);
	return out;
}

Mat testBlur(Mat src, Size ksize)
{
	Mat out;
	blur(src, out, ksize);
	return out;
}

Mat testGaussianBlur(Mat src, Size ksize)
{
	Mat out;
	GaussianBlur(src, out, ksize, 0);
	return out;
}

Mat testMedianBlur(Mat src, int ksize)
{
	Mat out;
	medianBlur(src, out, ksize);
	return out;
}

Mat testBilateralBlur(Mat src, int ksize, double sigma_color, double sigma_space)
{
	Mat out;
	bilateralFilter(src, out, ksize, sigma_color, sigma_space);
	return out;
}

int main()
{
	Mat img = imread("../test image/lena.jpg"); // 读入一张图片
	namedWindow("Src image");
	imshow("Src image", img);	// 显示原图

	Mat box_img = testBoxBlur(img, Size(7, 7), false);	// 调用函数，取得方框滤波图像
	namedWindow("Box image");
	imshow("Box image", box_img);	// 显示模糊之后的图像

	Mat blur_img = testBlur(img, Size(7, 7));	// 调用函数，取得均值滤波图像
	namedWindow("Blur image");
	imshow("Blur image", blur_img);	// 显示模糊之后的图像

	Mat gaussion_img = testGaussianBlur(img, Size(7, 7));	// 调用函数，取得高斯滤波图像
	namedWindow("Gaussion image");
	imshow("Gaussion image", gaussion_img);	// 显示模糊之后的图像

	Mat median_img = testMedianBlur(img, 7);	// 调用函数，取得中值滤波图像
	namedWindow("Median image");
	imshow("Median image", median_img);	// 显示模糊之后的图像

	Mat bilateral_img = testBilateralBlur(img, 7, 7 * 2, 7 / 2);	// 调用函数，取得双边滤波图像
	namedWindow("Bilateral image");
	imshow("Bilateral image", bilateral_img);	// 显示模糊之后的图像

	waitKey(0);
	return 0;
}

