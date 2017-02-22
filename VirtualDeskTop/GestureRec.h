#pragma once
class GestureRec{
protected:
	unsigned char *img;
	unsigned int width, height;

public:
	GestureRec();
	~GestureRec();

	virtual void setImg(unsigned char *_img, unsigned int _width, unsigned int _heigth);
	virtual unsigned char* getGesture(unsigned char &type, float &Zoom, float &x, float &y) = 0;
};

