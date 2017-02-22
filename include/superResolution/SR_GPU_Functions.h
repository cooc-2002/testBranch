#pragma once

#define Memory_Size 1024
#define Block_Size 8
#define Thread_Size 128

void bindTexture(unsigned char* image, int width, int height);
void bindTexture_gray(float* d_gray_img, int width, int height);

__global__ void rgbToGray(unsigned char* input, int height, int width, float* output);
__global__ void Hessian(float* input, int height, int width, float* output);
__global__ void Interpolation(unsigned char* input, float* e_vec, int height, int width, unsigned char* output);
__global__ void ucharCopy(float* d_result_img, int height, int width,unsigned char* d_result_img_uchar);

__global__ void rgbToGray_tex(int height, int width, float* output);
__global__ void Hessian_tex(int height, int width, float* output);
__global__ void Interpolation_tex(float* e_vec, int height, int width, unsigned char* output);
