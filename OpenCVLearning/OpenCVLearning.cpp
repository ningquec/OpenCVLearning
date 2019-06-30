// OpenCVLearning.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <opencv2/opencv.hpp> // 导入OpenCV头文件
#include <iostream>

using namespace cv;	// 使用命名空间cv
using namespace std;

void testRefCount(Mat img)
{
	Mat A = imread("../test image/lena.jpg", 0);
	cout << "count 1: " << *A.refcount << endl;
	Mat B = A;
	// A 的引用计数+1，为2
	cout << "count 2: " << *A.refcount << endl;
}

void test1()	// 通过访问行首像素地址来遍历
{
	Mat image = imread("../test image/lena.jpg");
	namedWindow("Src image");
	imshow("Src image", image);

	uchar div = 2;
	for (int j = 0; j < image.rows; j++)
	{
		uchar *data = image.ptr<uchar>(j);
		for (int i = 0; i < image.cols * image.channels(); i++)
		{
			data[i] = data[i] / div;
		}
	}
	namedWindow("Result image");
	imshow("Result image", image);

	waitKey(0);
}

void test2()	// 通过迭代器访问像素
{
	Mat image = imread("../test image/lena.jpg");
	namedWindow("Src image");
	imshow("Src image", image);

	uchar div = 2;
	MatIterator_<Vec3b> it, end;
	it = image.begin<Vec3b>();
	end = image.end<Vec3b>();

	for (; it < end; it++)
	{
		(*it)[0] = (*it)[0] / div;
		(*it)[1] = (*it)[1] / div;
		(*it)[2] = (*it)[2] / div;
	}
	namedWindow("Result image");
	imshow("Result image", image);

	waitKey(0);
}

void test3()	// 通过at函数
{
	Mat image = imread("../test image/lena.jpg");
	namedWindow("Src image");
	imshow("Src image", image);

	uchar div = 2;
	for(int j = 0; j < image.rows; j++)
	{
		for(int i = 0; i < image.cols; i++)
		{
			image.at<Vec3b>(j, i)[0] = image.at<Vec3b>(j, i)[0] / div;
			image.at<Vec3b>(j, i)[1] = image.at<Vec3b>(j, i)[1] / div;
			image.at<Vec3b>(j, i)[2] = image.at<Vec3b>(j, i)[2] / div;
		}
	}
	namedWindow("Result image");
	imshow("Result image", image);

	waitKey(0);
}

int main()
{
	test2();
	return 0;
}

