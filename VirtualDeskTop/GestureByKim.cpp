#include "GestureByKim.h"
#include "Gesture.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

GestureByKim::GestureByKim(){
	pGesture = new Gesture;
	mImg = NULL;
}

GestureByKim::~GestureByKim(){}

void GestureByKim::setImg(unsigned char *_img, unsigned int _width, unsigned int _heigth) {
	width = _width;
	height = _heigth;
	img = _img;

	//if (mImg == NULL) {
	//	mImg = new cv::Mat(height, width, CV_8UC3);
	//}
	//else if (width * height != mImg->rows * mImg->cols) {
	//	delete mImg;
	//	mImg = new cv::Mat(height, width, CV_8UC3);
	//}

	//for (int i = 0; i < width * height; i++) {
	//	mImg->data[i * 3] = img[i*4];
	//	mImg->data[i * 3 + 1] = img[i * 4 + 1];
	//	mImg->data[i * 3 + 2] = img[i * 4 + 2];
	//}
}

unsigned char* GestureByKim::getGesture(unsigned char &type, float &Zoom, float &x, float &y) {
	cv::Mat im_origin(height, width, CV_8UC3);

	for (int i = 0; i < width * height; i++) {
		im_origin.data[i * 3] = img[i * 4];
		im_origin.data[i * 3 + 1] = img[i * 4 + 1];
		im_origin.data[i * 3 + 2] = img[i * 4 + 2];
	}

	pGesture->gesture_main(im_origin, bOverview, bZoom, bRelocal, fZoomMag, fDeltaX, fDeltaY, 1);

	type = bOverview + (bZoom < 1) + (bRelocal < 2);
	x = fDeltaX;
	y = fDeltaY;
	Zoom = fZoomMag;

	return 0;
}