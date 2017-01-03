#pragma once
#include <Windows.h>

class ScreenCopy
{
private:
	HDC hdcScreen;
	HDC hdcMemDC;
	HBITMAP hbmScreen;
	BITMAP bmpScreen;
	HANDLE hDIB;
	BITMAPINFOHEADER bi;

public:
	unsigned char *screenData;
	unsigned int height, width;

	ScreenCopy();
	~ScreenCopy();

	void ScreenUpdate();
};

