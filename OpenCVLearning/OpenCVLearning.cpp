

#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

const int CV_QR_NORTH = 0;
const int CV_QR_EAST = 1;
const int CV_QR_SOUTH = 2;
const int CV_QR_WEST = 3;

float cv_distance(Point2f P, Point2f Q);					
float cv_lineEquation(Point2f L, Point2f M, Point2f J);		
float cv_lineSlope(Point2f L, Point2f M, int& alignement);	
void cv_getVertices(vector<vector<Point> > contours, int c_id, float slope, vector<Point2f>& X);
void cv_updateCorner(Point2f P, Point2f ref, float& baseline, Point2f& corner);
void cv_updateCornerOr(int orientation, vector<Point2f> IN, vector<Point2f> &OUT);
bool getIntersectionPoint(Point2f a1, Point2f a2, Point2f b1, Point2f b2, Point2f& intersection);
float cross(Point2f v1, Point2f v2);

int main(int argc, char **argv)
{

	VideoCapture capture(0);
	Mat image;

	if (!capture.isOpened()) {
		cerr << " ERR: Unable find input Video source." << endl;
		return -1;
	}

	capture >> image;
	if (image.empty()) {
		cerr << "ERR: Unable to query image from capture device.\n" << endl;
		return -1;
	}

	Mat gray(image.size(), CV_MAKETYPE(image.depth(), 1));			
	Mat edges(image.size(), CV_MAKETYPE(image.depth(), 1));	
	Mat gaussian;
	Mat qr, qr_raw, qr_gray, qr_thres;

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	int mark, A, B, C, top, right, bottom, median1, median2, outlier;
	float AB, BC, CA, dist, slope, areat, arear, areab, large, padding;

	int align, orientation;

	int key = 0;
	while (key != 'q')		// 键入q退出		
	{
		qr_raw = Mat::zeros(100, 100, CV_8UC3);
		qr = Mat::zeros(100, 100, CV_8UC3);
		qr_gray = Mat::zeros(100, 100, CV_8UC1);
		qr_thres = Mat::zeros(100, 100, CV_8UC1);

		capture >> image;						// 导入一幅图像

		// 预处理
		cvtColor(image, gray, CV_RGB2GRAY);
		GaussianBlur(gray, gaussian, Size(3, 3), 0);
		Canny(gaussian, edges, 100, 200, 3);

		// 查找轮廓
		findContours(edges, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); 

		mark = 0;	

		vector<Moments> mu(contours.size());
		vector<Point2f> mc(contours.size());

		for (int i = 0; i < contours.size(); i++)
		{
			// 计算图像的质心，并保存到mc中，每一个轮廓有一个质心
			mu[i] = moments(contours[i], false);
			mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		}

		// 找到三个定位矩形
		for (int i = 0; i < contours.size(); i++)
		{
			int k = i;
			int c = 0;
			while (hierarchy[k][2] != -1)
			{
				k = hierarchy[k][2];
				c = c + 1;
			}
			if (c >= 5)
			{
				if (mark == 0)		A = i;
				else if (mark == 1)	B = i;	
				else if (mark == 2)	C = i;		
				mark = mark + 1;
			}
		}

		if (mark >= 3)	
		{
			// 判断三点距离，来得到两个对顶角的点和一个中间点
			AB = cv_distance(mc[A], mc[B]);
			BC = cv_distance(mc[B], mc[C]);
			CA = cv_distance(mc[C], mc[A]);

			if (AB > BC && AB > CA)
			{
				outlier = C; median1 = A; median2 = B;
			}
			else if (CA > AB && CA > BC)
			{
				outlier = B; median1 = A; median2 = C;
			}
			else if (BC > AB && BC > CA)
			{
				outlier = A;  median1 = B; median2 = C;
			}

			top = outlier;						

			// 计算dist和slope，判断二维码朝向
			dist = cv_lineEquation(mc[median1], mc[median2], mc[outlier]);	
			slope = cv_lineSlope(mc[median1], mc[median2], align);

			if (align == 0)
			{
				bottom = median1;
				right = median2;
			}
			else if (slope < 0 && dist < 0)		// Orientation - North
			{
				bottom = median1;
				right = median2;
				orientation = CV_QR_NORTH;
			}
			else if (slope > 0 && dist < 0)		// Orientation - East
			{
				right = median1;
				bottom = median2;
				orientation = CV_QR_EAST;
			}
			else if (slope < 0 && dist > 0)		// Orientation - South			
			{
				right = median1;
				bottom = median2;
				orientation = CV_QR_SOUTH;
			}

			else if (slope > 0 && dist > 0)		// Orientation - West
			{
				bottom = median1;
				right = median2;
				orientation = CV_QR_WEST;
			}

			float area_top, area_right, area_bottom;

			if (top < contours.size() && right < contours.size() && bottom < contours.size() && contourArea(contours[top]) > 10 && contourArea(contours[right]) > 10 && contourArea(contours[bottom]) > 10)
			{

				vector<Point2f> L, M, O, tempL, tempM, tempO;
				Point2f N;

				vector<Point2f> src, dst;

				Mat warp_matrix;

				// 计算出三个矩形个最远的点
				cv_getVertices(contours, top, slope, tempL);
				cv_getVertices(contours, right, slope, tempM);
				cv_getVertices(contours, bottom, slope, tempO);

				cv_updateCornerOr(orientation, tempL, L); 			
				cv_updateCornerOr(orientation, tempM, M); 			
				cv_updateCornerOr(orientation, tempO, O); 			

				// 根据三个最远的点推算出第四个点的位置
				int iflag = getIntersectionPoint(M[1], M[2], O[3], O[2], N);

				src.push_back(L[0]);
				src.push_back(M[1]);
				src.push_back(N);
				src.push_back(O[3]);

				dst.push_back(Point2f(0, 0));
				dst.push_back(Point2f(qr.cols, 0));
				dst.push_back(Point2f(qr.cols, qr.rows));
				dst.push_back(Point2f(0, qr.rows));

				if (src.size() == 4 && dst.size() == 4)			
				{
					// 投影变换，将视频中的图像，投影到一个（0， 0， 100， 100）的窗口内
					warp_matrix = getPerspectiveTransform(src, dst);
					warpPerspective(image, qr_raw, warp_matrix, Size(qr.cols, qr.rows));
					copyMakeBorder(qr_raw, qr, 10, 10, 10, 10, BORDER_CONSTANT, Scalar(255, 255, 255));

					cvtColor(qr, qr_gray, CV_RGB2GRAY);
					threshold(qr_gray, qr_thres, 127, 255, CV_THRESH_BINARY);
				}

				// 在原图上绘制找到的轮廓
				drawContours(image, contours, top, Scalar(255, 200, 0), 2, 8, hierarchy, 0);
				drawContours(image, contours, right, Scalar(0, 0, 255), 2, 8, hierarchy, 0);
				drawContours(image, contours, bottom, Scalar(255, 0, 100), 2, 8, hierarchy, 0);
			}
		}

		imshow("Image", image);
		imshow("QR code", qr_thres);

		key = waitKey(1);	// 等待键盘输入

	}
	return 0;
}


float cv_distance(Point2f P, Point2f Q)
{
	return sqrt(pow(abs(P.x - Q.x), 2) + pow(abs(P.y - Q.y), 2));
}

float cv_lineEquation(Point2f L, Point2f M, Point2f J)
{
	float a, b, c, pdist;

	a = -((M.y - L.y) / (M.x - L.x));
	b = 1.0;
	c = (((M.y - L.y) / (M.x - L.x)) * L.x) - L.y;

	pdist = (a * J.x + (b * J.y) + c) / sqrt((a * a) + (b * b));
	return pdist;
}

float cv_lineSlope(Point2f L, Point2f M, int& alignement)
{
	float dx, dy;
	dx = M.x - L.x;
	dy = M.y - L.y;

	if (dy != 0)
	{
		alignement = 1;
		return (dy / dx);
	}
	else		
	{
		alignement = 0;
		return 0.0;
	}
}

// Function: Routine to calculate 4 Corners of the Marker in Image Space using Region partitioning
// Theory: OpenCV Contours stores all points that describe it and these points lie the perimeter of the polygon.
//	The below function chooses the farthest points of the polygon since they form the vertices of that polygon,
//	exactly the points we are looking for. To choose the farthest point, the polygon is divided/partitioned into
//	4 regions equal regions using bounding box. Distance algorithm is applied between the centre of bounding box
//	every contour point in that region, the farthest point is deemed as the vertex of that region. Calculating
//	for all 4 regions we obtain the 4 corners of the polygon ( - quadrilateral).
void cv_getVertices(vector<vector<Point>> contours, int c_id, float slope, vector<Point2f>& quad)
{
	Rect box;
	box = boundingRect(contours[c_id]);

	Point2f M0, M1, M2, M3;
	Point2f A, B, C, D, W, X, Y, Z;

	A = box.tl();
	B.x = box.br().x;
	B.y = box.tl().y;
	C = box.br();
	D.x = box.tl().x;
	D.y = box.br().y;

	W.x = (A.x + B.x) / 2;
	W.y = A.y;

	X.x = B.x;
	X.y = (B.y + C.y) / 2;

	Y.x = (C.x + D.x) / 2;
	Y.y = C.y;

	Z.x = D.x;
	Z.y = (D.y + A.y) / 2;

	float dmax[4];
	dmax[0] = 0.0;
	dmax[1] = 0.0;
	dmax[2] = 0.0;
	dmax[3] = 0.0;

	float pd1 = 0.0;
	float pd2 = 0.0;

	if (slope > 5 || slope < -5)
	{

		for (int i = 0; i < contours[c_id].size(); i++)
		{
			pd1 = cv_lineEquation(C, A, contours[c_id][i]);	// Position of point w.r.t the diagonal AC 
			pd2 = cv_lineEquation(B, D, contours[c_id][i]);	// Position of point w.r.t the diagonal BD

			if ((pd1 >= 0.0) && (pd2 > 0.0))
			{
				cv_updateCorner(contours[c_id][i], W, dmax[1], M1);
			}
			else if ((pd1 > 0.0) && (pd2 <= 0.0))
			{
				cv_updateCorner(contours[c_id][i], X, dmax[2], M2);
			}
			else if ((pd1 <= 0.0) && (pd2 < 0.0))
			{
				cv_updateCorner(contours[c_id][i], Y, dmax[3], M3);
			}
			else if ((pd1 < 0.0) && (pd2 >= 0.0))
			{
				cv_updateCorner(contours[c_id][i], Z, dmax[0], M0);
			}
			else
				continue;
		}
	}
	else
	{
		int halfx = (A.x + B.x) / 2;
		int halfy = (A.y + D.y) / 2;

		for (int i = 0; i < contours[c_id].size(); i++)
		{
			if ((contours[c_id][i].x < halfx) && (contours[c_id][i].y <= halfy))
			{
				cv_updateCorner(contours[c_id][i], C, dmax[2], M0);
			}
			else if ((contours[c_id][i].x >= halfx) && (contours[c_id][i].y < halfy))
			{
				cv_updateCorner(contours[c_id][i], D, dmax[3], M1);
			}
			else if ((contours[c_id][i].x > halfx) && (contours[c_id][i].y >= halfy))
			{
				cv_updateCorner(contours[c_id][i], A, dmax[0], M2);
			}
			else if ((contours[c_id][i].x <= halfx) && (contours[c_id][i].y > halfy))
			{
				cv_updateCorner(contours[c_id][i], B, dmax[1], M3);
			}
		}
	}

	quad.push_back(M0);
	quad.push_back(M1);
	quad.push_back(M2);
	quad.push_back(M3);

}


void cv_updateCorner(Point2f P, Point2f ref, float& baseline, Point2f& corner)
{
	float temp_dist;
	temp_dist = cv_distance(P, ref);

	if (temp_dist > baseline)
	{
		baseline = temp_dist;	
		corner = P;						
	}

}
void cv_updateCornerOr(int orientation, vector<Point2f> IN, vector<Point2f> &OUT)
{
	Point2f M0, M1, M2, M3;
	if (orientation == CV_QR_NORTH)
	{
		M0 = IN[0];
		M1 = IN[1];
		M2 = IN[2];
		M3 = IN[3];
	}
	else if (orientation == CV_QR_EAST)
	{
		M0 = IN[1];
		M1 = IN[2];
		M2 = IN[3];
		M3 = IN[0];
	}
	else if (orientation == CV_QR_SOUTH)
	{
		M0 = IN[2];
		M1 = IN[3];
		M2 = IN[0];
		M3 = IN[1];
	}
	else if (orientation == CV_QR_WEST)
	{
		M0 = IN[3];
		M1 = IN[0];
		M2 = IN[1];
		M3 = IN[2];
	}

	OUT.push_back(M0);
	OUT.push_back(M1);
	OUT.push_back(M2);
	OUT.push_back(M3);
}

bool getIntersectionPoint(Point2f a1, Point2f a2, Point2f b1, Point2f b2, Point2f& intersection)
{
	Point2f p = a1;
	Point2f q = b1;
	Point2f r(a2 - a1);
	Point2f s(b2 - b1);

	if (cross(r, s) == 0) { return false; }

	float t = cross(q - p, s) / cross(r, s);

	intersection = p + t * r;
	return true;
}

float cross(Point2f v1, Point2f v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}
