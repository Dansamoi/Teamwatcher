#ifndef SCREEN
#define SCREEN

#pragma once
#include <windows.h>
#include <math.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <memory>
#include <windows.h>

//using byte = unsigned char;

class Screen
{
public:
	static HBITMAP GetScreenShot(void)
    {
        // This function is to get screenshot of the Screen
        // returns HBITNAP 

        int w = GetSystemMetrics(SM_CXSCREEN);

        int h = GetSystemMetrics(SM_CYSCREEN);

        // copy screen to bitmap
        HDC hScreen = GetDC(NULL);
        HDC hDC = CreateCompatibleDC(hScreen);
        //HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
        HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
        HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
       // BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
        BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, 0, 0, SRCCOPY);

        // clean up
        SelectObject(hDC, old_obj);
        DeleteDC(hDC);
        ReleaseDC(NULL, hScreen);

        // save bitmap to clipboard
        return hBitmap;
    }

    static HBITMAP ResizeImage(HBITMAP hBitmap, int w, int h) {
        // Returns a resized HBITMAP
        HANDLE ret = CopyImage(hBitmap, IMAGE_BITMAP, w, h, LR_COPYDELETEORG);
        return (HBITMAP)ret;
    }

    static BOOL DrawBitmap(HDC hDC, INT x, INT y, INT width, INT height, HBITMAP hBitmap, DWORD dwROP)
    {
        // Drawing HBITMAP on screen
        HDC hDCBits;
        BITMAP Bitmap;
        BOOL bResult;

        if (!hDC || !hBitmap)
            return FALSE;

        // get HDC
        hDCBits = CreateCompatibleDC(hDC);
        GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
        SelectObject(hDCBits, hBitmap);

        SetStretchBltMode(hDC, HALFTONE);

        // Stretching Image according to screen
        bResult = StretchBlt(hDC, x, y, width, height,
            hDCBits, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, dwROP);
        DeleteDC(hDCBits);
        ::DeleteObject(hBitmap);

        return bResult;
    }

    static HBITMAP HBITMAPFromPixels(const std::vector<uint8_t>& Pixels, int width, int height, int BitsPerPixel)
    {
        // Function returns a vector of bytes
        // of HBITMAP pixels
        BITMAPINFO Info = { 0 };
        std::memset(&Info, 0, sizeof(BITMAPINFO));

        Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        Info.bmiHeader.biWidth = width;
        Info.bmiHeader.biHeight = height;
        Info.bmiHeader.biPlanes = 1;
        Info.bmiHeader.biBitCount = BitsPerPixel;
        Info.bmiHeader.biCompression = BI_RGB;
        Info.bmiHeader.biSizeImage = ((width * BitsPerPixel + 31) / 32) * 4 * height;

        HDC hdc = GetDC(nullptr);
        HBITMAP Result = CreateDIBitmap(hdc, &Info.bmiHeader, CBM_INIT, &Pixels[0], &Info, DIB_RGB_COLORS);
        DeleteDC(hdc);
        return Result;
    }

    static void HBITMAPToPixels(HBITMAP BitmapHandle, std::vector<uint8_t>& Pixels, int width, int height, int BitsPerPixel)
    {
        // Function returns a HBITMAP from vector of bytes of pixels
        Pixels.clear();
        BITMAP Bmp = { 0 };
        BITMAPINFO Info = { 0 };
        HDC DC = CreateCompatibleDC(nullptr);
        std::memset(&Info, 0, sizeof(BITMAPINFO));
        HBITMAP OldBitmap = (HBITMAP)SelectObject(DC, BitmapHandle);
        GetObject(BitmapHandle, sizeof(Bmp), &Bmp);

        Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        Info.bmiHeader.biWidth = width = Bmp.bmWidth;
        Info.bmiHeader.biHeight = height = Bmp.bmHeight;
        Info.bmiHeader.biPlanes = 1;
        Info.bmiHeader.biBitCount = BitsPerPixel = Bmp.bmBitsPixel;
        Info.bmiHeader.biCompression = BI_RGB;
        Info.bmiHeader.biSizeImage = ((width * Bmp.bmBitsPixel + 31) / 32) * 4 * height;

        Pixels.resize(Info.bmiHeader.biSizeImage);
        GetDIBits(DC, BitmapHandle, 0, height, &Pixels[0], &Info, DIB_RGB_COLORS);
        SelectObject(DC, OldBitmap);
        height = height < 0 ? -height : height;
        DeleteDC(DC);
    }

};

#endif // SCREEN

