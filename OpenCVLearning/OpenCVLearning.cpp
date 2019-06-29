// OpenCVLearning.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <opencv2/opencv.hpp> // 导入OpenCV头文件

using namespace cv;	// 使用命名空间cv

int main()
{
	Mat img = imread("../test image/blob.jpg", 0); // 以灰度图的格式读入一张图片
	namedWindow("Src image");
	imshow("Src image", img);	// 显示原图

	threshold(img, img, 100, 255, THRESH_BINARY);	// 进行一次二值化操作

	Mat element = getStructuringElement(MORPH_RECT, Size(11, 11));	//创建一个卷积核
	Mat dilate_img, erode_img;
	dilate(img, dilate_img, element); //进行膨胀操作
	erode(img, erode_img, element); // 进行腐蚀操作

	namedWindow("Dilate image");
	imshow("Dilate image", dilate_img);
	namedWindow("Erode image");
	imshow("Erode image", erode_img);

	Mat open_img;
	morphologyEx(img, open_img, MORPH_OPEN, element);	// 开操作
	namedWindow("Open image");
	imshow("Open image", open_img);

	Mat close_img;
	morphologyEx(img, close_img, MORPH_CLOSE, element);	// 闭操作
	namedWindow("Close image");
	imshow("Close image", close_img);

	Mat grad_img;
	morphologyEx(img, grad_img, MORPH_GRADIENT, element);	// 形态学梯度
	namedWindow("Gradiant image");
	imshow("Gradiant image", grad_img);

	Mat top_img;
	morphologyEx(img, top_img, MORPH_TOPHAT, element);	//顶帽
	namedWindow("Top hat image");
	imshow("Top hat image", top_img);

	Mat black_img;
	morphologyEx(img, black_img, MORPH_BLACKHAT, element);	// 黑帽
	namedWindow("Black hat image");
	imshow("Black hat image", black_img);

	waitKey(0);

	return 0;
}

