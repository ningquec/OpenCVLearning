// OpenCVLearning.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "plate_locate.h"

using namespace cv;


int main()
{
	Mat src = imread("../test image/car.jpg");
	namedWindow("src");
	imshow("src", src);	// 显示原图

	PlateLocate pl;
	pl.process(src);	// 调用车牌定位类的处理方法，寻找车牌

	waitKey(0);
	return 0;
}

