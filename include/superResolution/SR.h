#pragma once

class SR {
protected:
	unsigned char *image;
	unsigned char *output;
	int width, height, pixelNum;

public:
	SR();
	virtual ~SR();

	virtual void setImage(unsigned char* image, int _width, int _height) = 0;
	virtual unsigned char* perform() = 0;
};
