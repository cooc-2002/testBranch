#pragma once
#include "SR.h"

class SR_GPU2: public SR {
private:
	unsigned char* d_img;
	float* d_gray_img;
	float* d_e_vec;
	unsigned char * d_result_img;

public:
	SR_GPU2();
	~SR_GPU2();

	virtual void setImage(unsigned char* image, int _width, int _height);
	virtual unsigned char* perform();
};
