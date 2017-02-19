#define _CRT_SECURE_NO_WARNINGS	// ignore sprintf error
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#include "Gesture.h"

using namespace std;
using namespace cv;

int main()
{
	
	// video image -----
	int v_num = 0;
	VideoCapture video_in;
	if (!video_in.open(1)) {
		printf("Error ErrorErrorErrorErrorErrorErrorErrorat video_in open.\n");
		return -1;
	}
	video_in.set(CV_CAP_PROP_EXPOSURE, -5);
	//VideoWriter video_out;
	//sprintf(filepath, "./v_result_%02d.avi", v_num);
	//video_out.open(filepath, CV_FOURCC('M', 'J', 'P', 'G'), 30, Size(video_in.get(CV_CAP_PROP_FRAME_WIDTH), video_in.get(CV_CAP_PROP_FRAME_HEIGHT)), true);
	//if (!video_out.isOpened()) {
	//	printf("Error at video_out open.\n");
	//	return -1;
	//}

	Mat im_origin;
	int frame = 0;
	Gesture mGesture;
	bool bOverview, bZoom, bRelocal;
	float fZoomMag, fDeltaX, fDeltaY;
	while(1) {
		//printf("Frame number: %d\n", ++frame);

		video_in >> im_origin;
		mGesture.gesture_main(im_origin, bOverview, bZoom, bRelocal, fZoomMag, fDeltaX, fDeltaY, 1);
		waitKey(5);
		
		//video_out << im_out_kf;
	}
	//video_in.release();
	//video_out.release();
	return 0;

}
