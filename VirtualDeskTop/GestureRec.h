#pragma once
class GestureRec
{
private:
	unsigned char img;
	unsigned int width, height;

public:
	GestureRec();
	~GestureRec();

	void setImg(unsigned char _img, unsigned int _width, unsigned int _heigth);
	virtual void getGesture(unsigned char &type, float &Zoom, float &x, float &y) = 0;
};

