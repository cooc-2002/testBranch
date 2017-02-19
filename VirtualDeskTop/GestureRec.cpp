#include "GestureRec.h"

GestureRec::GestureRec(){
	img = 0;
	width = 0; height = 0;
}

GestureRec::~GestureRec(){

}

void GestureRec::setImg(unsigned char _img, unsigned int _width, unsigned int _heigth) {
	img = _img;
	width = _width;
	height = _heigth;
}
