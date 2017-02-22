#include <stdio.h>
#include <stdlib.h>
#include "superResolution/SR_GPU2.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda_profiler_api.h>
#include "superResolution/SR_GPU_Functions.h"

//texture<unsigned char,2, cudaReadModeNormalizedFloat> image_TM;
texture<unsigned char,2, cudaReadModeElementType> image_TM;
texture<unsigned char,2, cudaReadModeElementType> gray_TM;

__global__ void rgbToGray(unsigned char* input, int height, int width, float* output){	// input:RGB image, output:gray_image
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int index = x*width+y;

	if (x>=0 && x<height && y>= 0 && y<width)
		output[index] = float(16.0f + (65.738f*input[index*3]+129.057f*input[index*3 +1]+25.064f*input[index*3 + 2])/256.0f +0.5f);
}

__global__ void Hessian(float* input, int height, int width, float* output){ //input:gray_image, output: e_vec
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;

	float dxx_Hessian, dyy_Hessian, dxy_Hessian, e_val;

	if (x>=1 && x<height-1 && y>= 1 && y<width-1){
		dxx_Hessian = input[x*width + y+1] + input[x*width + y-1] - 2*input[x*width + y];
		dyy_Hessian = input[(x+1)*width + y] + input[(x-1)*width + y] - 2*input[x*width + y];
		dxy_Hessian = (input[(x-1)*width + (y-1)] + input[(x+1)*width + (y+1)] - input[(x+1)*width + (y-1)] - input[(x-1)*width + (y+1)])/4.0f;

		e_val = 0.5f * (dxx_Hessian + dyy_Hessian + sqrtf((dxx_Hessian - dyy_Hessian)*(dxx_Hessian - dyy_Hessian) + 4*dxy_Hessian*dxy_Hessian));
		if (abs(dxy_Hessian) <= 0.01 || abs(e_val - dxx_Hessian) <= 0.01){
			output[2*(x*width + y)] = 1;
			output[2*(x*width + y)+1] = 0;
		}else{
			output[2*(x*width + y)] = dxy_Hessian;
			output[2*(x*width + y)+1] = e_val - dxx_Hessian;
		}
	}
}

__global__ void Interpolation(unsigned char* input, float* e_vec, int height, int width, unsigned char *output){ // input:image, output: result_image
	__shared__ float shared[Thread_Size*3*2][3];

	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;

	//////////////////////////////////////////////////
	//////// I. Save shared memory part ////////
	//////////////////////////////////////////////////

	shared[6*(threadIdx.y/3)+(threadIdx.y%3)][0] = input[3*(width*x+(blockIdx.y*blockDim.y))+threadIdx.y];
	shared[6*((threadIdx.y+Thread_Size)/3)+((threadIdx.y+Thread_Size)%3)][0] = input[3*(width*x+(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size)];
	shared[6*((threadIdx.y+Thread_Size*2)/3)+((threadIdx.y+Thread_Size*2)%3)][0] = input[3*(width*x+(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*2)];

	shared[6*(threadIdx.y/3)+(threadIdx.y%3)][2] = input[3*(width*(x+1)+(blockIdx.y*blockDim.y))+threadIdx.y];
	shared[6*((threadIdx.y+Thread_Size)/3)+((threadIdx.y+Thread_Size)%3)][2] = input[3*(width*(x+1)+(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size)];
	shared[6*((threadIdx.y+Thread_Size*2)/3)+((threadIdx.y+Thread_Size*2)%3)][2] = input[3*(width*(x+1)+(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*2)];

//	shared[6*(threadIdx.y/3)+(threadIdx.y%3)][2] = tex2D(image_TM,3*y+0,x);
//	shared[6*((threadIdx.y+Thread_Size)/3)+((threadIdx.y+Thread_Size)%3)][2] = tex2D(image_TM,3*y+1,x);
//	shared[6*((threadIdx.y+Thread_Size*2)/3)+((threadIdx.y+Thread_Size*2)%3)][2] = tex2D(image_TM,3*y+2,x);

	__syncthreads();

	///////////////////////////////////////////////////
	//////////// II. Processing part ////////////
	///////////////////////////////////////////////////

	float a=abs(e_vec[2*(x*width + y)]);
	float b=abs(e_vec[2*(x*width + y)+1]);
	float ori = b/a;

	float P1, P2;

	if (ori>=1){
		P1 = (b-a)/(2*b);
		P2 = (a+b)/(2*b);
	}
	else if(ori>=0 && ori< 1){
		P1 = (a+b)/(2*a);
		P2 = (a-b)/(2*a);
	}
	else if(ori>=-1 && ori< 0){
		P1 = (a+b)/(2*a);
		P2 = (a-b)/(2*a);
	}
	else{
		P1 = (a+b)/(2*b);
		P2 = (b-a)/(2*b);
	}

	if (abs(ori)>=1){
		if (threadIdx.y < Thread_Size-1){
			shared[threadIdx.y*6 + 3][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 0][0] + shared[threadIdx.y*6 + 6 + 0][2]) + 0.5 * P2 * (shared[threadIdx.y*6 + 0][2] + shared[threadIdx.y*6 + 6 + 0][0]);
			shared[threadIdx.y*6 + 4][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 1][0] + shared[threadIdx.y*6 + 6 + 1][2]) + 0.5 * P2 * (shared[threadIdx.y*6 + 1][2] + shared[threadIdx.y*6 + 6 + 1][0]);
			shared[threadIdx.y*6 + 5][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 2][0] + shared[threadIdx.y*6 + 6 + 2][2]) + 0.5 * P2 * (shared[threadIdx.y*6 + 2][2] + shared[threadIdx.y*6 + 6 + 2][0]);
		}
		else{
			shared[threadIdx.y*6 + 3][1] = P1 * shared[threadIdx.y*6 + 0][0] + P2 * shared[threadIdx.y*6 + 0][2];
			shared[threadIdx.y*6 + 4][1] = P1 * shared[threadIdx.y*6 + 1][0] + P2 * shared[threadIdx.y*6 + 1][2];
			shared[threadIdx.y*6 + 5][1] = P1 * shared[threadIdx.y*6 + 2][0] + P2 * shared[threadIdx.y*6 + 2][2];
		}
	}
	else{
		if (threadIdx.y < Thread_Size-1){
			shared[threadIdx.y*6 + 3][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 0][2] + shared[threadIdx.y*6 + 6 + 0][0]) + 0.5 * P2 * (shared[threadIdx.y*6 + 6 + 0][2] + shared[threadIdx.y*6 + 0][0]);
			shared[threadIdx.y*6 + 4][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 1][2] + shared[threadIdx.y*6 + 6 + 1][0]) + 0.5 * P2 * (shared[threadIdx.y*6 + 6 + 1][2] + shared[threadIdx.y*6 + 1][0]);
			shared[threadIdx.y*6 + 5][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 2][2] + shared[threadIdx.y*6 + 6 + 2][0]) + 0.5 * P2 * (shared[threadIdx.y*6 + 6 + 2][2] + shared[threadIdx.y*6 + 2][0]);
		}
		else{
			shared[threadIdx.y*6 + 3][1] = P1 * (shared[threadIdx.y*6 + 0][2]) + P2 * shared[threadIdx.y*6 + 0][0];
			shared[threadIdx.y*6 + 4][1] = P1 * (shared[threadIdx.y*6 + 1][2]) + P2 * shared[threadIdx.y*6 + 1][0];
			shared[threadIdx.y*6 + 5][1] = P1 * (shared[threadIdx.y*6 + 2][2]) + P2 * shared[threadIdx.y*6 + 2][0];
		}
	}

	__syncthreads();

	// horizontal

	if (threadIdx.y < Thread_Size-1){

		a = sqrtf((shared[threadIdx.y*6 + 0][0] - shared[threadIdx.y*6 + 3][1]) * (shared[threadIdx.y*6 + 0][0] - shared[threadIdx.y*6 + 3][1]) +
				   (shared[threadIdx.y*6 + 1][0] - shared[threadIdx.y*6 + 4][1]) * (shared[threadIdx.y*6 + 1][0] - shared[threadIdx.y*6 + 4][1]) +
				   (shared[threadIdx.y*6 + 2][0] - shared[threadIdx.y*6 + 5][1]) * (shared[threadIdx.y*6 + 2][0] - shared[threadIdx.y*6 + 5][1]) );
		b = sqrtf((shared[threadIdx.y*6 + 6 + 0][0] - shared[threadIdx.y*6 + 3][1]) * (shared[threadIdx.y*6 + 6 + 0][0] - shared[threadIdx.y*6 + 3][1]) +
				   (shared[threadIdx.y*6 + 6 + 1][0] - shared[threadIdx.y*6 + 4][1]) * (shared[threadIdx.y*6 + 6 + 1][0] - shared[threadIdx.y*6 + 4][1]) +
				   (shared[threadIdx.y*6 + 6 + 2][0] - shared[threadIdx.y*6 + 5][1]) * (shared[threadIdx.y*6 + 6 + 2][0] - shared[threadIdx.y*6 + 5][1]) );

		if (a==0 && b==0){
			P1 = 0.5;
			P2 = 0.5;
		}
		else if (a!=0 && b==0){
			P1 = 0;
			P2 = 1;
		}
		else if(a==0 && b!=0){
			P1 = 1;
			P2 = 0;
		}
		else{
			P1 = b/(a+b);
			P2 = a/(a+b);
		}

		shared[threadIdx.y*6 + 3][0] = P1 * shared[threadIdx.y*6 + 0][0] + P2 * shared[threadIdx.y*6 + 6 + 0][0];
		shared[threadIdx.y*6 + 4][0] = P1 * shared[threadIdx.y*6 + 1][0] + P2 * shared[threadIdx.y*6 + 6 + 1][0];
		shared[threadIdx.y*6 + 5][0] = P1 * shared[threadIdx.y*6 + 2][0] + P2 * shared[threadIdx.y*6 + 6 + 2][0];
	}
	else{
		shared[threadIdx.y*6 + 3][0] = shared[threadIdx.y*6 + 0][0];
		shared[threadIdx.y*6 + 4][0] = shared[threadIdx.y*6 + 1][0];
		shared[threadIdx.y*6 + 5][0] = shared[threadIdx.y*6 + 2][0];
	}

	// vertical

	if (threadIdx.y < Thread_Size-1){

		a = sqrtf((shared[threadIdx.y*6 + 0][2] - shared[threadIdx.y*6 + 3][1]) * (shared[threadIdx.y*6 + 0][2] - shared[threadIdx.y*6 + 3][1]) +
				   (shared[threadIdx.y*6 + 1][2] - shared[threadIdx.y*6 + 4][1]) * (shared[threadIdx.y*6 + 1][2] - shared[threadIdx.y*6 + 4][1]) +
				   (shared[threadIdx.y*6 + 2][2] - shared[threadIdx.y*6 + 5][1]) * (shared[threadIdx.y*6 + 2][2] - shared[threadIdx.y*6 + 5][1]) );
		b = sqrtf((shared[threadIdx.y*6 + 0][0] - shared[threadIdx.y*6 + 3][1]) * (shared[threadIdx.y*6 + 0][0] - shared[threadIdx.y*6 + 3][1]) +
				   (shared[threadIdx.y*6 + 1][0] - shared[threadIdx.y*6 + 4][1]) * (shared[threadIdx.y*6 + 1][0] - shared[threadIdx.y*6 + 4][1]) +
				   (shared[threadIdx.y*6 + 2][0] - shared[threadIdx.y*6 + 5][1]) * (shared[threadIdx.y*6 + 2][0] - shared[threadIdx.y*6 + 5][1]) );

		if (a==0 && b==0){
			P1 = 0.5;
			P2 = 0.5;
		}
		else if (a!=0 && b==0){
			P1 = 1;
			P2 = 0;
		}
		else if(a==0 && b!=0){
			P1 = 0;
			P2 = 1;
		}
		else{
			P1 = b/(a+b);
			P2 = a/(a+b);
		}

		shared[threadIdx.y*6 + 0][1] = P1 * shared[threadIdx.y*6 + 0][2] + P2 * shared[threadIdx.y*6 + 0][0];
		shared[threadIdx.y*6 + 1][1] = P1 * shared[threadIdx.y*6 + 1][2] + P2 * shared[threadIdx.y*6 + 1][0];
		shared[threadIdx.y*6 + 2][1] = P1 * shared[threadIdx.y*6 + 2][2] + P2 * shared[threadIdx.y*6 + 2][0];
	}
	else{
		shared[threadIdx.y*6 + 0][1] = shared[threadIdx.y*6 + 0][0];
		shared[threadIdx.y*6 + 1][1] = shared[threadIdx.y*6 + 1][0];
		shared[threadIdx.y*6 + 2][1] = shared[threadIdx.y*6 + 2][0];
	}
	__syncthreads();

	//////////////////////////////////////////////////
	/////// III. Load shared memory part ///////
	//////////////////////////////////////////////////

	if(y < width){
		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y)] = shared[threadIdx.y][0];
		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size)] = shared[threadIdx.y+Thread_Size][0];
		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*2)] = shared[threadIdx.y+Thread_Size*2][0];

		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*3)] = shared[threadIdx.y+Thread_Size*3][0];
		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*4)] = shared[threadIdx.y+Thread_Size*4][0];
		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*5)] = shared[threadIdx.y+Thread_Size*5][0];

		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y)] = shared[threadIdx.y][1];
		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size)] = shared[threadIdx.y+Thread_Size][1];
		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*2)] = shared[threadIdx.y+Thread_Size*2][1];

		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*3)] = shared[threadIdx.y+Thread_Size*3][1];
		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*4)] = shared[threadIdx.y+Thread_Size*4][1];
		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*5)] = shared[threadIdx.y+Thread_Size*5][1];
	}
}

__global__ void ucharCopy(float* d_result_img, int height, int width, unsigned char* d_result_img_uchar){
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;

	if (x<=(2*height-1) && y<=(2*width-1)){
		d_result_img_uchar[3*(x*(2*width)+y)  ] = d_result_img[3*(x*(2*width)+y)  ];
		d_result_img_uchar[3*(x*(2*width)+y)+1] = d_result_img[3*(x*(2*width)+y)+1];
		d_result_img_uchar[3*(x*(2*width)+y)+2] = d_result_img[3*(x*(2*width)+y)+2];
	}
}








//////////////////////////////////////
/////Texture Memory Function/////
//////////////////////////////////////

__global__ void rgbToGray_tex(int height, int width, float* output){	// input:RGB image, output:gray_image
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int index = x*width+y;

	if (x>=0 && x<height && y>= 0 && y<width)
		//tex2D(gray_TM, y, x) = float(16.0f + (65.738f*tex2D(image_TM, 3*y+0, x)+129.057f*tex2D(image_TM, 3*y+1, x)+25.064f*tex2D(image_TM, 3*y+2, x))/256.0f +0.5f);
		output[index] = float(16.0f + (65.738f*tex2D(image_TM, 3*y+0, x)+129.057f*tex2D(image_TM, 3*y+1, x)+25.064f*tex2D(image_TM, 3*y+2, x))/256.0f +0.5f);
}

__global__ void Hessian_tex(int height, int width, float* output){ //input:gray_image, output: e_vec
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;

	float dxx_Hessian, dyy_Hessian, dxy_Hessian, e_val;

	if (x>=1 && x<height-1 && y>= 1 && y<width-1){
		dxx_Hessian = tex2D(gray_TM, y+1, x) + tex2D(gray_TM, y-1, x) - 2*tex2D(gray_TM, y, x);
		dyy_Hessian = tex2D(gray_TM, y, x+1) + tex2D(gray_TM, y, x-1) - 2*tex2D(gray_TM, y, x);
		dxy_Hessian = (tex2D(gray_TM, y-1, x-1) + tex2D(gray_TM, y+1, x+1) - tex2D(gray_TM, y-1, x+1) - tex2D(gray_TM, y+1, x-1))/4.0f;

		e_val = 0.5f * (dxx_Hessian + dyy_Hessian + sqrtf((dxx_Hessian - dyy_Hessian)*(dxx_Hessian - dyy_Hessian) + 4*dxy_Hessian*dxy_Hessian));
		if (abs(dxy_Hessian) <= 0.01 || abs(e_val - dxx_Hessian) <= 0.01){
			output[2*(x*width + y)] = 1;
			output[2*(x*width + y)+1] = 0;
		}else{
			output[2*(x*width + y)] = dxy_Hessian;
			output[2*(x*width + y)+1] = e_val - dxx_Hessian;
		}
	}
}

__global__ void Interpolation_tex(float* e_vec, int height, int width, unsigned char *output){ // input:image, output: result_image
	__shared__ float shared[Thread_Size*3*2];

	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;

	//////////////////////////////////////////////////
	//////// I. Save shared memory part ////////
	//////////////////////////////////////////////////

	shared[6*(threadIdx.y/3)+(threadIdx.y%3)] = tex2D(image_TM, 3*(blockIdx.y*blockDim.y)+threadIdx.y, x);
	shared[6*((threadIdx.y+Thread_Size)/3)+((threadIdx.y+Thread_Size)%3)] = tex2D(image_TM, 3*(blockIdx.y*blockDim.y)+threadIdx.y+Thread_Size, x);
	shared[6*((threadIdx.y+Thread_Size*2)/3)+((threadIdx.y+Thread_Size*2)%3)] = tex2D(image_TM, 3*(blockIdx.y*blockDim.y)+threadIdx.y+Thread_Size*2, x);

	__syncthreads();

	///////////////////////////////////////////////////
	//////////// II. Processing part ////////////
	///////////////////////////////////////////////////

	float a=abs(e_vec[2*(x*width + y)]);
	float b=abs(e_vec[2*(x*width + y)+1]);
	float ori = b/a;

	float P1, P2;

	if (ori>=1){
		P2 = (b-a)/(2*b);
		P1 = (a+b)/(2*b);
	}
	else if(ori>=0 && ori< 1){
		P1 = (a+b)/(2*a);
		P2 = (a-b)/(2*a);
	}
	else if(ori>=-1 && ori< 0){
		P1 = (a+b)/(2*a);
		P2 = (a-b)/(2*a);
	}
	else{
		P2 = (a+b)/(2*b);
		P1 = (b-a)/(2*b);
	}

	float diagonal[3];
	diagonal[0] = 0.5 * P1 * (tex2D(image_TM, 3*y+0, x) + tex2D(image_TM, 3*(y+1)+0, x+1)) + 0.5 * P2 * (tex2D(image_TM, 3*y+0, x+1) + tex2D(image_TM, 3*(y+1)+0, x));
	diagonal[1] = 0.5 * P1 * (tex2D(image_TM, 3*y+1, x) + tex2D(image_TM, 3*(y+1)+1, x+1)) + 0.5 * P2 * (tex2D(image_TM, 3*y+1, x+1) + tex2D(image_TM, 3*(y+1)+1, x));
	diagonal[2] = 0.5 * P1 * (tex2D(image_TM, 3*y+2, x) + tex2D(image_TM, 3*(y+1)+2, x+1)) + 0.5 * P2 * (tex2D(image_TM, 3*y+2, x+1) + tex2D(image_TM, 3*(y+1)+2, x));

	__syncthreads();

	// horizontal

	a = sqrtf((tex2D(image_TM, 3*y+0, x) - diagonal[0]) * (tex2D(image_TM, 3*y+0, x) - diagonal[0]) +
			   (tex2D(image_TM, 3*y+1, x) - diagonal[1]) * (tex2D(image_TM, 3*y+1, x) - diagonal[1]) +
			   (tex2D(image_TM, 3*y+2, x) - diagonal[2]) * (tex2D(image_TM, 3*y+2, x) - diagonal[2]));
	b = sqrtf((tex2D(image_TM, 3*(y+1)+0, x) - diagonal[0]) * (tex2D(image_TM, 3*(y+1)+0, x) - diagonal[0]) +
			   (tex2D(image_TM, 3*(y+1)+1, x) - diagonal[1]) * (tex2D(image_TM, 3*(y+1)+1, x) - diagonal[1]) +
			   (tex2D(image_TM, 3*(y+1)+2, x) - diagonal[2]) * (tex2D(image_TM, 3*(y+1)+2, x) - diagonal[2]));

	if (a==0 && b==0){
		P1 = 0.5;
		P2 = 0.5;
	}
	else if (a!=0 && b==0){
		P1 = 0;
		P2 = 1;
	}
	else if(a==0 && b!=0){
		P1 = 1;
		P2 = 0;
	}
	else{
		P1 = b/(a+b);
		P2 = a/(a+b);
	}

	shared[threadIdx.y*6 + 3] = P1 * tex2D(image_TM, 3*y+0, x) + P2 * tex2D(image_TM, 3*(y+1)+0, x);
	shared[threadIdx.y*6 + 4] = P1 * tex2D(image_TM, 3*y+1, x) + P2 * tex2D(image_TM, 3*(y+1)+1, x);
	shared[threadIdx.y*6 + 5] = P1 * tex2D(image_TM, 3*y+2, x) + P2 * tex2D(image_TM, 3*(y+1)+2, x);

	__syncthreads();

	if(y < width){
		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y)] = shared[threadIdx.y];
		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size)] = shared[threadIdx.y+Thread_Size];
		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*2)] = shared[threadIdx.y+Thread_Size*2];

		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*3)] = shared[threadIdx.y+Thread_Size*3];
		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*4)] = shared[threadIdx.y+Thread_Size*4];
		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*5)] = shared[threadIdx.y+Thread_Size*5];
	}

//	if(threadIdx.y == 0 && blockIdx.y == 0 && threadIdx.x == 0 && blockIdx.x == 0 ){
//		printf("shared[0] = %f\n", shared[0]);
//		printf("shared[1] = %f\n", shared[1]);
//		printf("shared[2] = %f\n", shared[2]);
//		printf("shared[3] = %f\n", shared[3]);
//		printf("shared[4] = %f\n", shared[4]);
//		printf("shared[5] = %f\n", shared[5]);
//	}

	// vertical

	a = sqrtf((tex2D(image_TM, 3*y+0, x+1) - diagonal[0]) * (tex2D(image_TM, 3*y+0, x+1) - diagonal[0]) +
			   (tex2D(image_TM, 3*y+1, x+1) - diagonal[1]) * (tex2D(image_TM, 3*y+1, x+1) - diagonal[1]) +
			   (tex2D(image_TM, 3*y+2, x+1) - diagonal[2]) * (tex2D(image_TM, 3*y+2, x+1) - diagonal[2]) );
	b = sqrtf((tex2D(image_TM, 3*y+0, x) - diagonal[0]) * (tex2D(image_TM, 3*y+0, x) - diagonal[0]) +
			   (tex2D(image_TM, 3*y+1, x) - diagonal[1]) * (tex2D(image_TM, 3*y+1, x) - diagonal[1]) +
			   (tex2D(image_TM, 3*y+2, x) - diagonal[2]) * (tex2D(image_TM, 3*y+2, x) - diagonal[2]) );

	if (a==0 && b==0){
		P1 = 0.5;
		P2 = 0.5;
	}
	else if (a!=0 && b==0){
		P1 = 1;
		P2 = 0;
	}
	else if(a==0 && b!=0){
		P1 = 0;
		P2 = 1;
	}
	else{
		P1 = b/(a+b);
		P2 = a/(a+b);
	}

	__syncthreads();

	shared[threadIdx.y*6 + 0] = P1 * tex2D(image_TM, 3*y+0, x+1) + P2 * tex2D(image_TM, 3*y+0, x);
	shared[threadIdx.y*6 + 1] = P1 * tex2D(image_TM, 3*y+1, x+1) + P2 * tex2D(image_TM, 3*y+1, x);
	shared[threadIdx.y*6 + 2] = P1 * tex2D(image_TM, 3*y+2, x+1) + P2 * tex2D(image_TM, 3*y+2, x);

	shared[threadIdx.y*6 + 3] = diagonal[0];
	shared[threadIdx.y*6 + 4] = diagonal[1];
	shared[threadIdx.y*6 + 5] = diagonal[2];

	__syncthreads();


	//////////////////////////////////////////////////
	/////// III. Load shared memory part ///////
	//////////////////////////////////////////////////

	if(y < width){
		output[3 * (2 * width*(2 * x + 1) + 2 * (blockIdx.y*blockDim.y)) + (threadIdx.y)] = shared[threadIdx.y];
		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size)] = shared[threadIdx.y+Thread_Size];
		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*2)] = shared[threadIdx.y+Thread_Size*2];

		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*3)] = shared[threadIdx.y+Thread_Size*3];
		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*4)] = shared[threadIdx.y+Thread_Size*4];
		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+Thread_Size*5)] = shared[threadIdx.y+Thread_Size*5];
	}
}


void bindTexture(unsigned char* image, int width, int height){
	// set up the CUDA array
	cudaChannelFormatDesc desc = cudaCreateChannelDesc<unsigned char>();
	cudaArray* texArray = 0;
	cudaMallocArray(&texArray, &desc, 3*width, height);
	cudaMemcpyToArray(texArray, 0,0, image, 3*width*height*sizeof(unsigned char),cudaMemcpyHostToDevice);

	// specify mutable texture reference parameters
//	image_TM.normalized = true;
//	image_TM.filterMode = cudaFilterModeLinear;
//	image_TM.addressMode[0] = cudaAddressModeBorder;
//	image_TM.addressMode[1] = cudaAddressModeBorder;

	// bind texture reference to array
	cudaBindTextureToArray(image_TM, texArray);
}

void bindTexture_gray(float* d_gray_img, int width, int height){
	// set up the CUDA array
	cudaChannelFormatDesc desc = cudaCreateChannelDesc<unsigned char>();
	cudaArray* texArray = 0;
	cudaMallocArray(&texArray, &desc, width, height);
	cudaMemcpyToArray(texArray, 0,0, d_gray_img, width*height*sizeof(unsigned char),cudaMemcpyHostToDevice);

	// specify mutable texture reference parameters
//	image_TM.normalized = true;
//	image_TM.filterMode = cudaFilterModeLinear;
//	image_TM.addressMode[0] = cudaAddressModeBorder;
//	image_TM.addressMode[1] = cudaAddressModeBorder;

	// bind texture reference to array
	cudaBindTextureToArray(gray_TM, texArray);
}
