#include "plate_locate.h"
#include <vector>

using namespace cv;
using namespace std;


void PlateLocate::process(cv::Mat img)
{
	// 1. 高斯滤波
	Mat gaussian;
	GaussianBlur(img, gaussian, Size(7, 7), 0);
	namedWindow("Gaussian image");
	imshow("Gaussian image", gaussian);

	// 2. 转灰度图
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	namedWindow("Gray image");
	imshow("Gray image", gray);

	// 3. Sobel算子
	Mat grad_x, grad_y, grad;
	Mat abs_grad_x, abs_grad_y;

	Sobel(gray, grad_x, CV_16S, 1, 0, 3);
	convertScaleAbs(grad_x, abs_grad_x);
	Sobel(gray, grad_y, CV_16S, 0, 1, 3);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	namedWindow("Sobel image");
	imshow("Sobel image", grad);

	// 4. 二值化
	Mat binary;
	threshold(grad, binary, 125, 255, CV_THRESH_BINARY);
	namedWindow("Binary image");
	imshow("Binary image", binary);

	// 5. 闭操作
	Mat close;
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	morphologyEx(binary, close, CV_MOP_CLOSE, element);
	namedWindow("Close image");
	imshow("Close image", close);

	// 6. 寻找边缘
	vector<vector<Point>> contours;
	findContours(close, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	// 7. 求外接矩形，并根据尺寸/角度筛选
	vector<vector<Point>>::iterator itc = contours.begin();
	vector<RotatedRect> rects;
	while (itc != contours.end())
	{
		RotatedRect mr = minAreaRect(Mat(*itc));
		if (!verifySize(mr) || !verifyAngle(mr))
		{
			itc = contours.erase(itc);
		}
		else
		{
			++itc;
			rects.push_back(mr);
			//setRotateRecToImg(img, mr);
		}
	}

	for (int i = 0; i < rects.size(); i++)
	{
		RotatedRect rec = rects.at(i);
		Mat m = rotateResult(img, rec);

		namedWindow("rec_img" + to_string(i));
		imshow("rec_img" + to_string(i), m);
	}
	
	namedWindow("Result image");
	imshow("Result image", img);
}

cv::Mat PlateLocate::rotateResult(cv::Mat image, cv::RotatedRect & rec)
{
	Mat rotate_mat = getRotationMatrix2D(		// 获取旋转矩阵
		rec.center,
		rec.angle, 1);
	Mat affine_result;
	warpAffine(image, affine_result, rotate_mat, image.size());// 仿射变换

	Mat rec_img = affine_result(Rect(rec.center.x - (rec.size.width / 2), rec.center.y - (rec.size.height / 2),
		rec.size.width, rec.size.height));
	

	return rec_img;


	//cv::Point2f* vertices = new cv::Point2f[4];
	//rec.points(vertices);
	//for (int j = 0; j < 4; j++)
	//{
	//	cv::line(image, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 0));
	//}

	//namedWindow("rotate");
	//imshow("rotate", image);
}

bool PlateLocate::verifySize(cv::RotatedRect & rec)
{
	float error = 0.6;
	float aspect = 3.75;

	int min = 44 * 14 * 3; 
	int max = 44 * 14 * 20; 
	float rmin = aspect - aspect * error;
	float rmax = aspect + aspect * error;

	int area = rec.size.height * rec.size.width;
	float r = (float)rec.size.width / (float)rec.size.height;
	if (r < 1)
	{
		r = (float)rec.size.height / (float)rec.size.width;
	}

	if ((area < min || area > max) || (r < rmin || r > rmax))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool PlateLocate::verifyAngle(cv::RotatedRect & rec)
{
	float r = (float)rec.size.width / (float)rec.size.height;
	float angle = rec.angle;
	Size rect_size = rec.size;
	if (r < 1)
	{
		angle = 90 + angle;
		swap(rect_size.width, rect_size.height);
	}
	return angle < 30 && angle > -30;
}

void PlateLocate::setRotateRecToImg(cv::Mat src, cv::RotatedRect & rec)
{
	cv::Point2f* vertices = new cv::Point2f[4];
	rec.points(vertices);
	for (int j = 0; j < 4; j++)
	{
		cv::line(src, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0, 255, 0));
	}
}
