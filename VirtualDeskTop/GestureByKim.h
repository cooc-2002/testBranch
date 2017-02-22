#pragma once
#include "GestureRec.h"

class Gesture;
namespace cv {
	class Mat;
}

class GestureByKim :
	public GestureRec
{
public:
	Gesture *pGesture;
	cv::Mat *mImg;

	bool bOverview, bZoom, bRelocal;
	float fZoomMag, fDeltaX, fDeltaY;

public:
	GestureByKim();
	~GestureByKim();

	virtual void setImg(unsigned char *_img, unsigned int _width, unsigned int _heigth);
	virtual unsigned char* getGesture(unsigned char &type, float &Zoom, float &x, float &y);
};

