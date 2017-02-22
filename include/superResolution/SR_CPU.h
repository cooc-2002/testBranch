#pragma once
#include "SR.h"

class SR_CPU: public SR {

private:
	float* image_R;
	float* image_G;
	float* image_B;
	float* e_vec2;
	float* ori;

public:
	SR_CPU();
	~SR_CPU();

	virtual void setImage(unsigned char* image, int _width, int _height);
	virtual unsigned char* perform();
};
