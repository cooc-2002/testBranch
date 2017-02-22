#define _CRT_SECURE_NO_WARNINGS	// ignore sprintf error
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <time.h>
#include "Gesture.h"
#include "ovrvision_pro.h"	//Ovrvision SDK

using namespace std;
using namespace cv;

int main()
{
	OVR::OvrvisionPro* pOvrvision = new OVR::OvrvisionPro();

	int locationID = 0;
	OVR::Camprop cameraMode = OVR::OV_CAMVR_FULL;
	//OVR::Camprop cameraMode = OVR::OV_CAMVR_VGA;
	if (pOvrvision->Open(locationID, cameraMode) == 0)
		printf("Ovrvision Pro Open Error!\nPlease check whether OvrvisionPro is connected.\n");
	else
		pOvrvision->SetCameraSyncMode(false);

	// video image -----
	int v_num = 0;
	VideoCapture video_in;
	if (!video_in.open(1)) {
		printf("Error ErrorErrorErrorErrorErrorErrorErrorat video_in open.\n");
		return -1;
	}
	//video_in.set(CV_CAP_PROP_EXPOSURE, -5);
	//VideoWriter video_out;
	//sprintf(filepath, "./v_result_%02d.avi", v_num);
	//video_out.open(filepath, CV_FOURCC('M', 'J', 'P', 'G'), 30, Size(video_in.get(CV_CAP_PROP_FRAME_WIDTH), video_in.get(CV_CAP_PROP_FRAME_HEIGHT)), true);
	//if (!video_out.isOpened()) {
	//	printf("Error at video_out open.\n");
	//	return -1;
	//}

	int frame = 0;
	Gesture mGesture;
	bool bOverview, bZoom, bRelocal;
	float fZoomMag, fDeltaX, fDeltaY;
	int width = pOvrvision->GetCamWidth();
	int height = pOvrvision->GetCamHeight();
	unsigned char *img;
	Mat im_origin(height, width, CV_8UC3);

	while(1) {
		//printf("Frame number: %d\n", ++frame);
		pOvrvision->PreStoreCamData(OVR::Camqt::OV_CAMQT_DMS);
		//pOvrvision->PreStoreCamData(OVR::Camqt::OV_CAMQT_DMSRMP);

		img = pOvrvision->GetCamImageBGRA(OVR::Cameye::OV_CAMEYE_LEFT);
		//im_origin.data = img;

		for (int i = 0; i < width * height; i++) {
			im_origin.data[i * 3] = img[i * 4];
			im_origin.data[i * 3 + 1] = img[i * 4 + 1];
			im_origin.data[i * 3 + 2] = img[i * 4 + 2];
		}

		//video_in >> im_origin;
		mGesture.gesture_main(im_origin, bOverview, bZoom, bRelocal, fZoomMag, fDeltaX, fDeltaY, 1);
		waitKey(5);
		
		//video_out << im_out_kf;
	}
	//video_in.release();
	//video_out.release();
	return 0;

}
