#include <stdio.h>
#include <stdlib.h>
#include "superResolution/SR_GPU2.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <cuda_profiler_api.h>
#include "superResolution/SR_GPU_Functions.h"

SR_GPU2::SR_GPU2() {
	d_gray_img = NULL;
	d_e_vec = NULL;
	d_result_img = NULL;

	cudaSetDevice(0);
	cudaProfilerStart();
	cudaDeviceSynchronize();
}

SR_GPU2::~SR_GPU2() {
	if (d_gray_img != NULL) cudaFree(d_gray_img);
	if (d_e_vec != NULL) cudaFree(d_e_vec);
	if (d_result_img != NULL) cudaFree(d_result_img);
}

void SR_GPU2::setImage(unsigned char* _image, int _width, int _height) {
	image = _image;
	width = _width;
	height = _height;

	if (pixelNum != width*height) {
		pixelNum = width*height;

		if (d_gray_img != NULL) cudaFree(d_gray_img);
		if (d_e_vec != NULL) cudaFree(d_e_vec);
		if (d_result_img != NULL) cudaFree(d_result_img);
		if (output != NULL) free(output);

		cudaMalloc((void**)&d_gray_img, height*width*sizeof(float));
		cudaMalloc((void**)&d_e_vec, height*width * 2 * sizeof(float));
		cudaMalloc((void**)&d_result_img, (2 * height + 1) * 2 * width * 3 * sizeof(unsigned char));
		output = (unsigned char*)malloc(2 * height * 2 * width * 3 * sizeof(unsigned char));
	}
}

unsigned char* SR_GPU2::perform() {
	// bind texture reference to array
	bindTexture(image, width, height);

	dim3 threads, numBlocks;
	threads.x = 1;
	threads.y = Thread_Size;
	numBlocks.x = height;
	numBlocks.y = width / threads.y + 1;

	rgbToGray_tex << <numBlocks, threads >> >(height, width, d_gray_img);
	cudaDeviceSynchronize();

	Hessian << <numBlocks, threads >> >(d_gray_img, height, width, d_e_vec);
	cudaDeviceSynchronize();

	Interpolation_tex << <numBlocks, threads >> >(d_e_vec, height, width, d_result_img);
	cudaDeviceSynchronize();
	cudaMemcpy(output, d_result_img, 2 * height * 2 * width * 3 * sizeof(unsigned char), cudaMemcpyDeviceToHost); //result_img

	cudaProfilerStop();

	return output;
}

