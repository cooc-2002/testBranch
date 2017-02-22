#include <stdio.h>
#include "superResolution/SR.h"

SR::SR(){
	image = NULL;
	output = NULL;
	width = 0;
	height = 0;
	pixelNum = 0;
}

SR::~SR(){
	if(output != NULL) free(output);
}
