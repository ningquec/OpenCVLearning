// OpenCVLearning.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <opencv2/opencv.hpp> // 导入OpenCV头文件

using namespace cv;	// 使用命名空间cv


int main()
{
	// 创建摄像头句柄，打开第0个设备
	VideoCapture cap(0);  
	Mat frame;
	while(true)
	{
		cap >> frame;	// 导出一张图像到frame中
		if (frame.data)
		{
			imshow("frame", frame);
		}
		waitKey(30); //0.03s获取一帧
	}
	return 0;
}

