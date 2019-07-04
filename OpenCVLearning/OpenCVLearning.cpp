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

	Mat affine_result;
	warpAffine(image, affine_result, rotate_mat, image.size());// 仿射变换
	namedWindow("Affine result");
	imshow("Affine result", affine_result);

	Point2f src_points[] = {
		cv::Point2f(165, 270),
		cv::Point2f(835, 270),
		cv::Point2f(360, 125),
		cv::Point2f(615, 125) };

	Point2f dst_points[] = {
		cv::Point2f(165, 270),
		cv::Point2f(835, 270),
		cv::Point2f(165, 30),
		cv::Point2f(835, 30) };

	// 获取投影变换矩阵
	Mat pers_mat = getPerspectiveTransform(src_points, dst_points);

	// 投影变换
	Mat pers_result;
	warpPerspective(image, pers_result, pers_mat, image.size());	
	namedWindow("Perspective result");
	imshow("Perspective result", pers_result);

	waitKey(0);
	return 0;
}

