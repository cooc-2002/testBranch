#include "ScreenCopy.h"

ScreenCopy::ScreenCopy()
{
	width = 1920;
	height = 1200;

	hdcMemDC = NULL;
	hbmScreen = NULL;

	// Create a compatible DC which is used in a BitBlt from the window DC
	hdcScreen = GetDC(NULL);
	hdcMemDC = CreateCompatibleDC(hdcScreen);

	// Create a compatible bitmap from the Window DC
	hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	screenData = new unsigned char[1920 * 1200 * 3];
}

ScreenCopy::~ScreenCopy()
{
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	DeleteDC(hdcScreen);

	delete[] screenData;
}

void ScreenCopy::ScreenUpdate() {
	SelectObject(hdcMemDC, hbmScreen);
	BitBlt(hdcMemDC, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

	CURSORINFO cursor = { sizeof(cursor) };
	::GetCursorInfo(&cursor);
	if (cursor.flags == CURSOR_SHOWING) {
		ICONINFOEXW info = { sizeof(info) };
		::GetIconInfoExW(cursor.hCursor, &info);
		const int x = cursor.ptScreenPos.x - info.xHotspot;
		const int y = cursor.ptScreenPos.y - info.yHotspot;
		BITMAP bmpCursor = { 0 };
		::GetObject(info.hbmColor, sizeof(bmpCursor), &bmpCursor);
		::DrawIconEx(hdcMemDC, x, y, cursor.hCursor, bmpCursor.bmWidth, bmpCursor.bmHeight,
			0, NULL, DI_NORMAL);
	}

	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);
	GetDIBits(hdcScreen, hbmScreen, 0, (UINT)bmpScreen.bmHeight,
		screenData, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

	GlobalUnlock(hDIB);
	GlobalFree(hDIB);
}
