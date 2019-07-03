

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

float cv_distance(Point2f P, Point2f Q);					// Get Distance between two points
float cv_lineEquation(Point2f L, Point2f M, Point2f J);		// Perpendicular Distance of a Point J from line formed by Points L and M; Solution to equation of the line Val = ax+by+c 
float cv_lineSlope(Point2f L, Point2f M, int& alignement);	// Slope of a line by two Points L and M on it; Slope of line, S = (x1 -x2) / (y1- y2)
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


	Mat gray(image.size(), CV_MAKETYPE(image.depth(), 1));			// To hold Grayscale Image
	Mat edges(image.size(), CV_MAKETYPE(image.depth(), 1));			// To hold Grayscale Image
	Mat traces(image.size(), CV_8UC3);								// For Debug Visuals
	Mat qr, qr_raw, qr_gray, qr_thres;

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	vector<Point> pointsseq;    //used to save the approximated sides of each contour

	int mark, A, B, C, top, right, bottom, median1, median2, outlier;
	float AB, BC, CA, dist, slope, areat, arear, areab, large, padding;

	int align, orientation;

	int key = 0;
	while (key != 'q')				// While loop to query for Image Input frame
	{
		traces = Scalar(0, 0, 0);
		qr_raw = Mat::zeros(100, 100, CV_8UC3);
		qr = Mat::zeros(100, 100, CV_8UC3);
		qr_gray = Mat::zeros(100, 100, CV_8UC1);
		qr_thres = Mat::zeros(100, 100, CV_8UC1);

		capture >> image;						// Capture Image from Image Input

		cvtColor(image, gray, CV_RGB2GRAY);		// Convert Image captured from Image Input to GrayScale	
		Canny(gray, edges, 100, 200, 3);		// Apply Canny edge detection on the gray image

		findContours(edges, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); // Find contours with hierarchy

		mark = 0;								// Reset all detected marker count for this frame

		vector<Moments> mu(contours.size());
		vector<Point2f> mc(contours.size());

		for (int i = 0; i < contours.size(); i++)
		{
			mu[i] = moments(contours[i], false);
			mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		}


		for (int i = 0; i < contours.size(); i++)
		{
			approxPolyDP(contours[i], pointsseq, arcLength(contours[i], true)*0.02, true);
			if (pointsseq.size() == 4)     
			{
				int k = i;
				int c = 0;

				while (hierarchy[k][2] != -1)
				{
					k = hierarchy[k][2];
					c = c + 1;
				}
				if (hierarchy[k][2] != -1)
					c = c + 1;

				if (c >= 5)
				{
					if (mark == 0)		A = i;
					else if (mark == 1)	B = i;		// i.e., A is already found, assign current contour to B
					else if (mark == 2)	C = i;		// i.e., A and B are already found, assign current contour to C
					mark = mark + 1;
				}
			}
		}


		if (mark >= 3)
		{
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

				vector<Point2f> src, dst;		// src - Source Points basically the 4 end co-ordinates of the overlay image
												// dst - Destination Points to transform overlay image	

				Mat warp_matrix;

				cv_getVertices(contours, top, slope, tempL);
				cv_getVertices(contours, right, slope, tempM);
				cv_getVertices(contours, bottom, slope, tempO);

				cv_updateCornerOr(orientation, tempL, L); 			// Re-arrange marker corners w.r.t orientation of the QR code
				cv_updateCornerOr(orientation, tempM, M); 			// Re-arrange marker corners w.r.t orientation of the QR code
				cv_updateCornerOr(orientation, tempO, O); 			// Re-arrange marker corners w.r.t orientation of the QR code

				int iflag = getIntersectionPoint(M[1], M[2], O[3], O[2], N);


				src.push_back(L[0]);
				src.push_back(M[1]);
				src.push_back(N);
				src.push_back(O[3]);

				dst.push_back(Point2f(0, 0));
				dst.push_back(Point2f(qr.cols, 0));
				dst.push_back(Point2f(qr.cols, qr.rows));
				dst.push_back(Point2f(0, qr.rows));

				if (src.size() == 4 && dst.size() == 4)			// Failsafe for WarpMatrix Calculation to have only 4 Points with src and dst
				{
					warp_matrix = getPerspectiveTransform(src, dst);
					warpPerspective(image, qr_raw, warp_matrix, Size(qr.cols, qr.rows));
					copyMakeBorder(qr_raw, qr, 10, 10, 10, 10, BORDER_CONSTANT, Scalar(255, 255, 255));

					cvtColor(qr, qr_gray, CV_RGB2GRAY);
					threshold(qr_gray, qr_thres, 127, 255, CV_THRESH_BINARY);
				}

				//Draw contours on the image
				drawContours(image, contours, top, Scalar(255, 200, 0), 2, 8, hierarchy, 0);
				drawContours(image, contours, right, Scalar(0, 0, 255), 2, 8, hierarchy, 0);
				drawContours(image, contours, bottom, Scalar(255, 0, 100), 2, 8, hierarchy, 0);
			}
		}

		imshow("Image", image);
		imshow("Traces", traces);
		imshow("QR code", qr_thres);

		key = waitKey(1);

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
	else				// Make sure we are not dividing by zero; so use 'alignement' flag
	{
		alignement = 0;
		return 0.0;
	}
}

void cv_getVertices(vector<vector<Point> > contours, int c_id, float slope, vector<Point2f>& quad)
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
		baseline = temp_dist;			// The farthest distance is the new baseline
		corner = P;						// P is now the farthest point
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
	return v1.x*v2.y - v1.y*v2.x;
}
