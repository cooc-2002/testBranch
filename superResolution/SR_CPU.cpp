#include <stdio.h>
#include <opencv2/imgproc.hpp>
#include "superResolution/SR_CPU.h"

SR_CPU::SR_CPU(){
	image_R = new float [height * width];
	image_G = new float [height * width];
	image_B = new float [height * width];

	float *e_vec2 = new float [ 2 * height * width ];
	memset(e_vec2, 0, 2*height*width);
	float *ori = new float [ height * width ];
	memset(ori, 0, height*width);
}

SR_CPU::~SR_CPU(){

}

void SR_CPU::setImage(unsigned char* _image, int _width, int _height){
	image = _image;
	width = _width;
	height = _height;

	if(pixelNum != width*height){
		pixelNum = width*height;

		image_R = new float [height * width];
		image_G = new float [height * width];
		image_B = new float [height * width];
		e_vec2 = new float [ 2 * height * width ];
		ori = new float [ height * width ];

		memset(e_vec2, 0, 2*height*width);
		memset(ori, 0, height*width);

		if(output != NULL) free(output);
		output = (unsigned char*)malloc(2*height*2*width*3*sizeof(unsigned char));
	}
}

unsigned char* SR_CPU::perform(){

	int x, y;

	for (x=0; x<height;x++){
		for (y=0; y<width;y++){
			image[x*width+y] = float(16 + (65.738*image[x*width*3 + y*3 + 2]+129.057*image[x*width*3 + y*3 + 1]+25.064*image[x*width*3 + y*3])/256 +0.5);
			image_R[x*width+y] = image[x*width*3 + y*3 + 2];
			image_G[x*width+y] = image[x*width*3 + y*3 + 1];
			image_B[x*width+y] = image[x*width*3 + y*3    ];
		}
	}

	float dxx_Hessian, dyy_Hessian, dxy_Hessian, e_val;

	for (x=1; x<height-1; x++){
		for(y = 1; y<width-1; y++){
			dxx_Hessian = image[x*width + y+1] + image[x*width + y-1] - 2*image[x*width + y];
			dyy_Hessian = image[(x+1)*width + y] + image[(x-1)*width + y] - 2*image[x*width + y];
			dxy_Hessian = (image[(x-1)*width + (y-1)] + image[(x+1)*width + (y+1)] - image[(x+1)*width + (y-1)] - image[(x-1)*width + (y+1)])/4;

			e_val = 1/2 * (dxx_Hessian + dyy_Hessian + sqrtf( pow(dxx_Hessian - dyy_Hessian,2) + 4*pow(dxy_Hessian,2)));
			if (dxy_Hessian == 0 && e_val - dxx_Hessian == 0){
				e_vec2[2*(x*width + y)] = 1;
				e_vec2[2*(x*width + y)+1] = 0;
			}else{
				e_vec2[2*(x*width + y)] = dxy_Hessian;
				e_vec2[2*(x*width + y)+1] = e_val - dxx_Hessian;
			}

			ori[x*width + y] = e_vec2[2*(x*width + y)+1]/e_vec2[2*(x*width + y)];
		}
	}

	float a,b;

	for (x=1; x<height-1; x++){
		for (y=1; y<width-1; y++){
			output[3*((2*x-1)*2*width+y*2-1)-1] = image_R[x*width+y];
			output[3*((2*x-1)*2*width+y*2+1)-1] = image_R[x*width+y+1];
			output[3*((2*x+1)*2*width+y*2-1)-1] = image_R[(x+1)*width+y];
			output[3*((2*x+1)*2*width+y*2+1)-1] = image_R[(x+1)*width+y+1];

			output[3*((2*x-1)*2*width+y*2-1)-2] = image_G[x*width+y];
			output[3*((2*x-1)*2*width+y*2+1)-2] = image_G[x*width+y+1];
			output[3*((2*x+1)*2*width+y*2-1)-2] = image_G[(x+1)*width+y];
			output[3*((2*x+1)*2*width+y*2+1)-2] = image_G[(x+1)*width+y+1];

			output[3*((2*x-1)*2*width+y*2-1)  ] = image_B[x*width+y];
			output[3*((2*x-1)*2*width+y*2+1)  ] = image_B[x*width+y+1];
			output[3*((2*x+1)*2*width+y*2-1)  ] = image_B[(x+1)*width+y];
			output[3*((2*x+1)*2*width+y*2+1)  ] = image_B[(x+1)*width+y+1];

			// diagonal
			a=abs(e_vec2[2*(x*width + y)]);
			b=abs(e_vec2[2*(x*width + y)+1]);

			if (ori[x*width + y]>=1){
				output[3*((2*x)*2*width+y*2)-1] = (b-a)/(2*b) * image_R[x*width+y] + (a+b)/(2*b) * image_R[(x+1)*width+y];
				output[3*((2*x)*2*width+y*2)-2] = (b-a)/(2*b) * image_G[x*width+y] + (a+b)/(2*b) * image_G[(x+1)*width+y];
				output[3*((2*x)*2*width+y*2)  ] = (b-a)/(2*b) * image_B[x*width+y] + (a+b)/(2*b) * image_B[(x+1)*width+y];
			}
			else if(ori[x*width + y]>=0 && ori[x*width + y]< 1){
				output[3*((2*x)*2*width+y*2)-1] = (a+b)/(2*a) * image_R[(x+1)*width+y+1] + (a-b)/(2*a) * image_R[(x+1)*width+y];
				output[3*((2*x)*2*width+y*2)-2] = (a+b)/(2*a) * image_G[(x+1)*width+y+1] + (a-b)/(2*a) * image_G[(x+1)*width+y];
				output[3*((2*x)*2*width+y*2)  ] = (a+b)/(2*a) * image_B[(x+1)*width+y+1] + (a-b)/(2*a) * image_B[(x+1)*width+y];
			}
			else if(ori[x*width + y]>=-1 && ori[x*width + y]< 0){
				output[3*((2*x)*2*width+y*2)-1] = (a+b)/(2*a) * image_R[(x+1)*width+y+1] + (a-b)/(2*a) * image_R[(x+1)*width+y];
				output[3*((2*x)*2*width+y*2)-2] = (a+b)/(2*a) * image_G[(x+1)*width+y+1] + (a-b)/(2*a) * image_G[(x+1)*width+y];
				output[3*((2*x)*2*width+y*2)  ] = (a+b)/(2*a) * image_B[(x+1)*width+y+1] + (a-b)/(2*a) * image_B[(x+1)*width+y];
			}
			else{
				output[3*((2*x)*2*width+y*2)-1] = (a+b)/(2*b) * image_R[x*width+y] + (b-a)/(2*b) * image_R[(x+1)*width+y];
				output[3*((2*x)*2*width+y*2)-2] = (a+b)/(2*b) * image_G[x*width+y] + (b-a)/(2*b) * image_G[(x+1)*width+y];
				output[3*((2*x)*2*width+y*2)  ] = (a+b)/(2*b) * image_B[x*width+y] + (b-a)/(2*b) * image_B[(x+1)*width+y];
			}

			// vertical
			a = abs(output[3*((2*x-1)*2*width+y*2-1)] - output[3*((2*x)*2*width+y*2)]);
			b = abs(output[3*((2*x+1)*2*width+y*2-1)] - output[3*((2*x)*2*width+y*2)]);
			if (a==0 && b==0){
				output[3*((2*x)*2*width+y*2-1)-1] = (output[3*((2*x-1)*2*width+y*2-1)-1]+output[3*((2*x+1)*2*width+y*2-1)-1])/2;
				output[3*((2*x)*2*width+y*2-1)-2] = (output[3*((2*x-1)*2*width+y*2-1)-2]+output[3*((2*x+1)*2*width+y*2-1)-2])/2;
				output[3*((2*x)*2*width+y*2-1)  ] = (output[3*((2*x-1)*2*width+y*2-1)  ]+output[3*((2*x+1)*2*width+y*2-1)  ])/2;
			}
			else if (a!=0 && b==0){
				output[3*((2*x)*2*width+y*2-1)-1] = output[3*((2*x+1)*2*width+y*2-1)-1];
				output[3*((2*x)*2*width+y*2-1)-2] = output[3*((2*x+1)*2*width+y*2-1)-2];
				output[3*((2*x)*2*width+y*2-1)  ] = output[3*((2*x+1)*2*width+y*2-1)  ];
			}
			else if(a==0 && b!=0){
				output[3*((2*x)*2*width+y*2-1)-1] = output[3*((2*x-1)*2*width+y*2-1)-1];
				output[3*((2*x)*2*width+y*2-1)-2] = output[3*((2*x-1)*2*width+y*2-1)-2];
				output[3*((2*x)*2*width+y*2-1)  ] = output[3*((2*x-1)*2*width+y*2-1)  ];
			}
			else{
				output[3*((2*x)*2*width+y*2-1)-1] = b/(a+b) * output[3*((2*x-1)*2*width+y*2-1)-1] + a/(a+b) * output[3*((2*x+1)*2*width+y*2-1)-1];
				output[3*((2*x)*2*width+y*2-1)-2] = b/(a+b) * output[3*((2*x-1)*2*width+y*2-1)-2] + a/(a+b) * output[3*((2*x+1)*2*width+y*2-1)-2];
				output[3*((2*x)*2*width+y*2-1)  ] = b/(a+b) * output[3*((2*x-1)*2*width+y*2-1)  ] + a/(a+b) * output[3*((2*x+1)*2*width+y*2-1)  ];
			}

			// horizontal
			a = abs(output[3*((2*x+1)*2*width+y*2-1)] - output[3*((2*x)*2*width+y*2)]);
			b = abs(output[3*((2*x+1)*2*width+y*2+1)] - output[3*((2*x)*2*width+y*2)]);
			if (a==0 && b==0){
				output[3*((2*x+1)*2*width+y*2)-1] = (output[3*((2*x+1)*2*width+y*2-1)-1]+output[3*((2*x+1)*2*width+y*2+1)-1])/2;
				output[3*((2*x+1)*2*width+y*2)-2] = (output[3*((2*x+1)*2*width+y*2-1)-2]+output[3*((2*x+1)*2*width+y*2+1)-2])/2;
				output[3*((2*x+1)*2*width+y*2)  ] = (output[3*((2*x+1)*2*width+y*2-1)  ]+output[3*((2*x+1)*2*width+y*2+1)  ])/2;
			}
			else if (a!=0 && b==0){
				output[3*((2*x+1)*2*width+y*2)-1] = output[3*((2*x+1)*2*width+y*2-1)-1];
				output[3*((2*x+1)*2*width+y*2)-2] = output[3*((2*x+1)*2*width+y*2-1)-2];
				output[3*((2*x+1)*2*width+y*2)  ] = output[3*((2*x+1)*2*width+y*2-1)  ];
			}
			else if(a==0 && b!=0){
				output[3*((2*x+1)*2*width+y*2)-1] = output[3*((2*x+1)*2*width+y*2+1)-1];
				output[3*((2*x+1)*2*width+y*2)-2] = output[3*((2*x+1)*2*width+y*2+1)-2];
				output[3*((2*x+1)*2*width+y*2)  ] = output[3*((2*x+1)*2*width+y*2+1)  ];
			}
			else{
				output[3*((2*x+1)*2*width+y*2)-1] = b/(a+b) * output[3*((2*x+1)*2*width+y*2-1)-1] + a/(a+b) * output[3*((2*x+1)*2*width+y*2+1)-1];
				output[3*((2*x+1)*2*width+y*2)-2] = b/(a+b) * output[3*((2*x+1)*2*width+y*2-1)-2] + a/(a+b) * output[3*((2*x+1)*2*width+y*2+1)-2];
				output[3*((2*x+1)*2*width+y*2)  ] = b/(a+b) * output[3*((2*x+1)*2*width+y*2-1)  ] + a/(a+b) * output[3*((2*x+1)*2*width+y*2+1)  ];
			}

		}
	}

	return output;
}
