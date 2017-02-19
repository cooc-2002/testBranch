#ifndef _BASIC_FUNC_H_
#define _BASIC_FUNC_H_

#include <stdio.h>
#include <stdlib.h>
#include <opencv.hpp>
#include <math.h>
#include <iostream>

#define PI 3.14159265
#define deg2rad(x) x/180*3.14159

using namespace std;
using namespace cv;

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

float cosd(float x) {
	return cos(x*PI/180);
}
float sind(float x) {
	return sin(x*PI/180);
}
void rectangle2(Mat &img, Point2f ctr, Size2f size, Scalar &color) {	// using center point and size
	rectangle(img, Point(ctr.x-size.width/2, ctr.y-size.height/2), Point(ctr.x+size.width/2, ctr.y+size.height/2), color);
}
void rectangle3(Mat &img, Point2f ctr, Size2f size, float theta, Scalar &color, int thickness = 1) {
	float w = size.width;
	float h = size.height;
	float hwcos = w/2*cos(theta);	// half width cos theta
	float hwsin = w/2*sin(theta);
	float hhcos = h/2*cos(theta);
	float hhsin = h/2*sin(theta);

	Point2f UL(ctr.x + hwsin - hhcos, ctr.y + hhsin + hwcos);	// upper left
	Point2f UR(ctr.x - hwsin - hhcos, ctr.y + hhsin - hwcos);
	Point2f LL(ctr.x + hwsin + hhcos, ctr.y - hhsin + hwcos);
	Point2f LR(ctr.x - hwsin + hhcos, ctr.y - hhsin - hwcos);	// lower right

	line(img, UL, UR, color, thickness);
	line(img, UR, LR, color, thickness);
	line(img, LR, LL, color, thickness);
	line(img, LL, UL, color, thickness);
}
void rectangle_from_4pt(Mat &img, Point2i *proj_pt, Scalar &color, int thickness = 1) {
	line(img, proj_pt[0], proj_pt[1], color, thickness);
	line(img, proj_pt[1], proj_pt[2], color, thickness);
	line(img, proj_pt[2], proj_pt[3], color, thickness);
	line(img, proj_pt[3], proj_pt[0], color, thickness);
}
void rotate_pt(Point2f ref_p, Point2f &trg_p, float deg) {
	/////////////??????????????????????????????????
	float x = trg_p.x - ref_p.x;
	float y = trg_p.y - ref_p.y;

	float x_rot = x*cosd(-deg) - y*sind(-deg);
	float y_rot = x*sind(-deg) + y*cosd(-deg);

	trg_p.x = x_rot + ref_p.x;
	trg_p.y = y_rot + ref_p.y;
}

#endif