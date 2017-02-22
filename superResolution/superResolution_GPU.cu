#include <stdio.h>
#include "superResolution/superResolution_GPU.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda_profiler_api.h>
#include <chrono>


#define Memory_Size 1024
#define Block_Size 8

__global__ void rgbToGray(unsigned char* input, int height, int width, float* output);
__global__ void Hessian(float* input, int height, int width, float* output);
__global__ void Interpolation(unsigned char* input, float* e_vec, int height, int width, unsigned char* output);
__global__ void ucharCopy(float* d_result_img, int height, int width,unsigned char* d_result_img_uchar);

unsigned char* superResolution_GPU(unsigned char* image0, int height, int width, int file_number){
	float sumTime;

	// Number of CUDA devices
	int devCount;
	cudaGetDeviceCount(&devCount);

	// Setting
	for (int i=0; i<devCount; ++i){
		cudaSetDevice(i);
		cudaDeviceReset();
	}
	cudaDeviceSynchronize();

	cudaProfilerStart();
	cudaDeviceSynchronize();

	std::chrono::system_clock::time_point startTime;
	std::chrono::microseconds endTime;
	startTime = std::chrono::system_clock::now();

	cudaStream_t stream1;
	cudaStreamCreate( &stream1);

	dim3 threads, numBlocks;
	threads.x = 1;
	threads.y = 128;
	numBlocks.x = height;
	numBlocks.y = width/threads.y + 1;

	///////////////////////////////////////
	//////    Convert RGB to gray    //////
	///////////////////////////////////////

	unsigned char* d_img;
	float* d_gray_img;
	cudaMalloc((void**) &d_img, (height+2)*width*3*sizeof(unsigned char));
	cudaMalloc((void**) &d_gray_img, height*width*sizeof(float));
	cudaMemcpy(d_img, image0,  height*width*3*sizeof(unsigned char), cudaMemcpyHostToDevice);
	rgbToGray<<<numBlocks,threads>>>(d_img, height, width, d_gray_img);
	cudaDeviceSynchronize();

	/////////////////////////////////////
	////     Hessian Analysis      //////
	/////////////////////////////////////

	float* d_e_vec;
	cudaMalloc((void**) &d_e_vec, height*width*2*sizeof(float));
	Hessian<<<numBlocks,threads>>>(d_gray_img, height, width, d_e_vec);
	cudaDeviceSynchronize();

	/////////////////////////////////////
	////       Interpolation       //////
	/////////////////////////////////////

	unsigned char * d_result_img;
	cudaMalloc((void**) &d_result_img, (2*height+1)*2*width*3*sizeof(unsigned char));
	Interpolation<<<numBlocks,threads>>>(d_img, d_e_vec, height, width, d_result_img);
	unsigned char* output = (unsigned char*)malloc(2*height*2*width*3*sizeof(unsigned char));
	cudaDeviceSynchronize();
	cudaMemcpy(output, d_result_img,  2*height*2*width*3*sizeof(unsigned char), cudaMemcpyDeviceToHost); //result_img

	cudaFree(d_img);
	cudaFree(d_gray_img);
	cudaFree(d_e_vec);
	cudaFree(d_result_img);

	endTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - startTime);
	sumTime += endTime.count()/1000.0f;
	printf("elapsed time = %3.3f\n", endTime.count()/1000.0f);
	cudaProfilerStop();

	return output;
}

//__global__ void rgbToGray(unsigned char* input, int height, int width, float* output){	// input:RGB image, output:gray_image
//	int x = threadIdx.x + blockIdx.x * blockDim.x;
//	int y = threadIdx.y + blockIdx.y * blockDim.y;
//	int index = x*width+y;
//
//	if (x>=0 && x<height && y>= 0 && y<width)
//		output[index] = float(16.0f + (65.738f*input[index*3]+129.057f*input[index*3 +1]+25.064f*input[index*3 + 2])/256.0f +0.5f);
//}
//
//__global__ void Hessian(float* input, int height, int width, float* output){ //input:gray_image, output: e_vec
//	int x = threadIdx.x + blockIdx.x * blockDim.x;
//	int y = threadIdx.y + blockIdx.y * blockDim.y;
//
//	float dxx_Hessian, dyy_Hessian, dxy_Hessian, e_val;
//
//	if (x>=1 && x<height-1 && y>= 1 && y<width-1){
//		dxx_Hessian = input[x*width + y+1] + input[x*width + y-1] - 2*input[x*width + y];
//		dyy_Hessian = input[(x+1)*width + y] + input[(x-1)*width + y] - 2*input[x*width + y];
//		dxy_Hessian = (input[(x-1)*width + (y-1)] + input[(x+1)*width + (y+1)] - input[(x+1)*width + (y-1)] - input[(x-1)*width + (y+1)])/4.0f;
//
//		e_val = 0.5f * (dxx_Hessian + dyy_Hessian + sqrtf((dxx_Hessian - dyy_Hessian)*(dxx_Hessian - dyy_Hessian) + 4*dxy_Hessian*dxy_Hessian));
//		if (abs(dxy_Hessian) <= 0.01 || abs(e_val - dxx_Hessian) <= 0.01){
//			output[2*(x*width + y)] = 1;
//			output[2*(x*width + y)+1] = 0;
//		}else{
//			output[2*(x*width + y)] = dxy_Hessian;
//			output[2*(x*width + y)+1] = e_val - dxx_Hessian;
//		}
//	}
//}
//
//__global__ void Interpolation(unsigned char* input, float* e_vec, int height, int width, unsigned char *output){ // input:image, output: result_image
//	__shared__ float shared[128*3*2][3];
//
//	int x = threadIdx.x + blockIdx.x * blockDim.x;
//	int y = threadIdx.y + blockIdx.y * blockDim.y;
//
//	//////////////////////////////////////////////////
//	//////// I. Save shared memory part ////////
//	//////////////////////////////////////////////////
//
//	shared[6*(threadIdx.y/3)+(threadIdx.y%3)][0] = input[3*(width*x+(blockIdx.y*blockDim.y))+threadIdx.y];
//	shared[6*((threadIdx.y+128)/3)+((threadIdx.y+128)%3)][0] = input[3*(width*x+(blockIdx.y*blockDim.y))+(threadIdx.y+128)];
//	shared[6*((threadIdx.y+256)/3)+((threadIdx.y+256)%3)][0] = input[3*(width*x+(blockIdx.y*blockDim.y))+(threadIdx.y+256)];
//
//	shared[6*(threadIdx.y/3)+(threadIdx.y%3)][2] = input[3*(width*(x+1)+(blockIdx.y*blockDim.y))+threadIdx.y];
//	shared[6*((threadIdx.y+128)/3)+((threadIdx.y+128)%3)][2] = input[3*(width*(x+1)+(blockIdx.y*blockDim.y))+(threadIdx.y+128)];
//	shared[6*((threadIdx.y+256)/3)+((threadIdx.y+256)%3)][2] = input[3*(width*(x+1)+(blockIdx.y*blockDim.y))+(threadIdx.y+256)];
//
//	__syncthreads();
//
//	///////////////////////////////////////////////////
//	//////////// II. Processing part ////////////
//	///////////////////////////////////////////////////
//
//	float a=abs(e_vec[2*(x*width + y)]);
//	float b=abs(e_vec[2*(x*width + y)+1]);
//	float ori = b/a;
//
//	float P1, P2;
//
//	if (ori>=1){
//		P1 = (b-a)/(2*b);
//		P2 = (a+b)/(2*b);
//	}
//	else if(ori>=0 && ori< 1){
//		P1 = (a+b)/(2*a);
//		P2 = (a-b)/(2*a);
//	}
//	else if(ori>=-1 && ori< 0){
//		P1 = (a+b)/(2*a);
//		P2 = (a-b)/(2*a);
//	}
//	else{
//		P1 = (a+b)/(2*b);
//		P2 = (b-a)/(2*b);
//	}
//
//	if (abs(ori)>=1){
//		// New algorithm
//		if (threadIdx.y < 127){
//			shared[threadIdx.y*6 + 3][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 0][0] + shared[threadIdx.y*6 + 6 + 0][2]) + 0.5 * P2 * (shared[threadIdx.y*6 + 0][2] + shared[threadIdx.y*6 + 6 + 0][0]);
//			shared[threadIdx.y*6 + 4][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 1][0] + shared[threadIdx.y*6 + 6 + 1][2]) + 0.5 * P2 * (shared[threadIdx.y*6 + 1][2] + shared[threadIdx.y*6 + 6 + 1][0]);
//			shared[threadIdx.y*6 + 5][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 2][0] + shared[threadIdx.y*6 + 6 + 2][2]) + 0.5 * P2 * (shared[threadIdx.y*6 + 2][2] + shared[threadIdx.y*6 + 6 + 2][0]);
//		}
//		else{
//			shared[threadIdx.y*6 + 3][1] = P1 * shared[threadIdx.y*6 + 0][0] + P2 * shared[threadIdx.y*6 + 0][2];
//			shared[threadIdx.y*6 + 4][1] = P1 * shared[threadIdx.y*6 + 1][0] + P2 * shared[threadIdx.y*6 + 1][2];
//			shared[threadIdx.y*6 + 5][1] = P1 * shared[threadIdx.y*6 + 2][0] + P2 * shared[threadIdx.y*6 + 2][2];
//		}
//	}
//	else{
//		// New algorithm
//		if (threadIdx.y < 127){
//			shared[threadIdx.y*6 + 3][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 0][2] + shared[threadIdx.y*6 + 6 + 0][0]) + 0.5 * P2 * (shared[threadIdx.y*6 + 6 + 0][2] + shared[threadIdx.y*6 + 0][0]);
//			shared[threadIdx.y*6 + 4][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 1][2] + shared[threadIdx.y*6 + 6 + 1][0]) + 0.5 * P2 * (shared[threadIdx.y*6 + 6 + 1][2] + shared[threadIdx.y*6 + 1][0]);
//			shared[threadIdx.y*6 + 5][1] = 0.5 * P1 * (shared[threadIdx.y*6 + 2][2] + shared[threadIdx.y*6 + 6 + 2][0]) + 0.5 * P2 * (shared[threadIdx.y*6 + 6 + 2][2] + shared[threadIdx.y*6 + 2][0]);
//		}
//		else{
//			shared[threadIdx.y*6 + 3][1] = P1 * (shared[threadIdx.y*6 + 0][2]) + P2 * shared[threadIdx.y*6 + 0][0];
//			shared[threadIdx.y*6 + 4][1] = P1 * (shared[threadIdx.y*6 + 1][2]) + P2 * shared[threadIdx.y*6 + 1][0];
//			shared[threadIdx.y*6 + 5][1] = P1 * (shared[threadIdx.y*6 + 2][2]) + P2 * shared[threadIdx.y*6 + 2][0];
//		}
//	}
//
//	__syncthreads();
//
//	// horizontal
//
//	if (threadIdx.y < 127){
////		a = sqrtf(powf(shared[threadIdx.y*6 + 0][0] - shared[threadIdx.y*6 + 3][1],2) +
////				   powf(shared[threadIdx.y*6 + 1][0] - shared[threadIdx.y*6 + 4][1],2) +
////				   powf(shared[threadIdx.y*6 + 2][0] - shared[threadIdx.y*6 + 5][1],2));
////		b = sqrtf(powf(shared[threadIdx.y*6 + 6 + 0][0] - shared[threadIdx.y*6 + 3][1],2) +
////				   powf(shared[threadIdx.y*6 + 6 + 1][0] - shared[threadIdx.y*6 + 4][1],2) +
////				   powf(shared[threadIdx.y*6 + 6 + 2][0] - shared[threadIdx.y*6 + 5][1],2));
//
//		a = sqrtf((shared[threadIdx.y*6 + 0][0] - shared[threadIdx.y*6 + 3][1]) * (shared[threadIdx.y*6 + 0][0] - shared[threadIdx.y*6 + 3][1]) +
//				   (shared[threadIdx.y*6 + 1][0] - shared[threadIdx.y*6 + 4][1]) * (shared[threadIdx.y*6 + 1][0] - shared[threadIdx.y*6 + 4][1]) +
//				   (shared[threadIdx.y*6 + 2][0] - shared[threadIdx.y*6 + 5][1]) * (shared[threadIdx.y*6 + 2][0] - shared[threadIdx.y*6 + 5][1]) );
//		b = sqrtf((shared[threadIdx.y*6 + 6 + 0][0] - shared[threadIdx.y*6 + 3][1]) * (shared[threadIdx.y*6 + 6 + 0][0] - shared[threadIdx.y*6 + 3][1]) +
//				   (shared[threadIdx.y*6 + 6 + 1][0] - shared[threadIdx.y*6 + 4][1]) * (shared[threadIdx.y*6 + 6 + 1][0] - shared[threadIdx.y*6 + 4][1]) +
//				   (shared[threadIdx.y*6 + 6 + 2][0] - shared[threadIdx.y*6 + 5][1]) * (shared[threadIdx.y*6 + 6 + 2][0] - shared[threadIdx.y*6 + 5][1]) );
//
//		if (a==0 && b==0){
//			P1 = 0.5;
//			P2 = 0.5;
//		}
//		else if (a!=0 && b==0){
//			P1 = 0;
//			P2 = 1;
//		}
//		else if(a==0 && b!=0){
//			P1 = 1;
//			P2 = 0;
//		}
//		else{
//			P1 = b/(a+b);
//			P2 = a/(a+b);
//		}
//
//		shared[threadIdx.y*6 + 3][0] = P1 * shared[threadIdx.y*6 + 0][0] + P2 * shared[threadIdx.y*6 + 6 + 0][0];
//		shared[threadIdx.y*6 + 4][0] = P1 * shared[threadIdx.y*6 + 1][0] + P2 * shared[threadIdx.y*6 + 6 + 1][0];
//		shared[threadIdx.y*6 + 5][0] = P1 * shared[threadIdx.y*6 + 2][0] + P2 * shared[threadIdx.y*6 + 6 + 2][0];
//	}
//	else{
//		shared[threadIdx.y*6 + 3][0] = shared[threadIdx.y*6 + 0][0];
//		shared[threadIdx.y*6 + 4][0] = shared[threadIdx.y*6 + 1][0];
//		shared[threadIdx.y*6 + 5][0] = shared[threadIdx.y*6 + 2][0];
//	}
//
//	//__syncthreads();
//
//	// vertical
//
//	if (threadIdx.y < 127){
////		a = sqrtf(powf(shared[threadIdx.y*6 + 0][2] - shared[threadIdx.y*6 + 3][1],2) +
////				   powf(shared[threadIdx.y*6 + 1][2] - shared[threadIdx.y*6 + 4][1],2) +
////				   powf(shared[threadIdx.y*6 + 2][2] - shared[threadIdx.y*6 + 5][1],2));
////		b = sqrtf(powf(shared[threadIdx.y*6 + 0][0] - shared[threadIdx.y*6 + 3][1],2) +
////				   powf(shared[threadIdx.y*6 + 1][0] - shared[threadIdx.y*6 + 4][1],2) +
////				   powf(shared[threadIdx.y*6 + 2][0] - shared[threadIdx.y*6 + 5][1],2));
//
//		a = sqrtf((shared[threadIdx.y*6 + 0][2] - shared[threadIdx.y*6 + 3][1]) * (shared[threadIdx.y*6 + 0][2] - shared[threadIdx.y*6 + 3][1]) +
//				   (shared[threadIdx.y*6 + 1][2] - shared[threadIdx.y*6 + 4][1]) * (shared[threadIdx.y*6 + 1][2] - shared[threadIdx.y*6 + 4][1]) +
//				   (shared[threadIdx.y*6 + 2][2] - shared[threadIdx.y*6 + 5][1]) * (shared[threadIdx.y*6 + 2][2] - shared[threadIdx.y*6 + 5][1]) );
//		b = sqrtf((shared[threadIdx.y*6 + 0][0] - shared[threadIdx.y*6 + 3][1]) * (shared[threadIdx.y*6 + 0][0] - shared[threadIdx.y*6 + 3][1]) +
//				   (shared[threadIdx.y*6 + 1][0] - shared[threadIdx.y*6 + 4][1]) * (shared[threadIdx.y*6 + 1][0] - shared[threadIdx.y*6 + 4][1]) +
//				   (shared[threadIdx.y*6 + 2][0] - shared[threadIdx.y*6 + 5][1]) * (shared[threadIdx.y*6 + 2][0] - shared[threadIdx.y*6 + 5][1]) );
//
//		if (a==0 && b==0){
//			P1 = 0.5;
//			P2 = 0.5;
//		}
//		else if (a!=0 && b==0){
//			P1 = 1;
//			P2 = 0;
//		}
//		else if(a==0 && b!=0){
//			P1 = 0;
//			P2 = 1;
//		}
//		else{
//			P1 = b/(a+b);
//			P2 = a/(a+b);
//		}
//
//		shared[threadIdx.y*6 + 0][1] = P1 * shared[threadIdx.y*6 + 0][2] + P2 * shared[threadIdx.y*6 + 0][0];
//		shared[threadIdx.y*6 + 1][1] = P1 * shared[threadIdx.y*6 + 1][2] + P2 * shared[threadIdx.y*6 + 1][0];
//		shared[threadIdx.y*6 + 2][1] = P1 * shared[threadIdx.y*6 + 2][2] + P2 * shared[threadIdx.y*6 + 2][0];
//	}
//	else{
//		shared[threadIdx.y*6 + 0][1] = shared[threadIdx.y*6 + 0][0];
//		shared[threadIdx.y*6 + 1][1] = shared[threadIdx.y*6 + 1][0];
//		shared[threadIdx.y*6 + 2][1] = shared[threadIdx.y*6 + 2][0];
//	}
//	__syncthreads();
//
//	//////////////////////////////////////////////////
//	/////// III. Load shared memory part ///////
//	//////////////////////////////////////////////////
//
//	if(y < width){
//		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y)] = shared[threadIdx.y][0];
//		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+128)] = shared[threadIdx.y+128][0];
//		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+256)] = shared[threadIdx.y+256][0];
//
//		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+384)] = shared[threadIdx.y+384][0];
//		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+512)] = shared[threadIdx.y+512][0];
//		output[3*(2*width*2*x+2*(blockIdx.y*blockDim.y))+(threadIdx.y+640)] = shared[threadIdx.y+640][0];
//
//		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y)] = shared[threadIdx.y][1];
//		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+128)] = shared[threadIdx.y+128][1];
//		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+256)] = shared[threadIdx.y+256][1];
//
//		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+384)] = shared[threadIdx.y+384][1];
//		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+512)] = shared[threadIdx.y+512][1];
//		output[3*(2*width*(2*x+1)+2*(blockIdx.y*blockDim.y))+(threadIdx.y+640)] = shared[threadIdx.y+640][1];
//	}
//}
//
//__global__ void ucharCopy(float* d_result_img, int height, int width, unsigned char* d_result_img_uchar){
//	int x = threadIdx.x + blockIdx.x * blockDim.x;
//	int y = threadIdx.y + blockIdx.y * blockDim.y;
//
//	if (x<=(2*height-1) && y<=(2*width-1)){
//		d_result_img_uchar[3*(x*(2*width)+y)  ] = d_result_img[3*(x*(2*width)+y)  ]; // (unsigned char)
//		d_result_img_uchar[3*(x*(2*width)+y)+1] = d_result_img[3*(x*(2*width)+y)+1];
//		d_result_img_uchar[3*(x*(2*width)+y)+2] = d_result_img[3*(x*(2*width)+y)+2];
//	}
//}
