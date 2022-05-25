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
        OpenClipboard(NULL);
        EmptyClipboard();
        SetClipboardData(CF_BITMAP, hBitmap);
        CloseClipboard();

        // clean up
        SelectObject(hDC, old_obj);
        DeleteDC(hDC);
        ReleaseDC(NULL, hScreen);
        DeleteObject(hBitmap);
    }

    static void SetImage(HWND hScreen, HBITMAP hImage){
        SendMessage(hScreen, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
    }

    static HBITMAP ResizeImage(HBITMAP hBitmap, int w, int h) {
        HANDLE ret = CopyImage(hBitmap, IMAGE_BITMAP, w, h, LR_COPYDELETEORG);
        return (HBITMAP)ret;
    }

    static BOOL DrawBitmap(HDC hDC, INT x, INT y, INT width, INT height, HBITMAP hBitmap, DWORD dwROP)
    {
        HDC hDCBits;
        BITMAP Bitmap;
        BOOL bResult;

        if (!hDC || !hBitmap)
            return FALSE;

        hDCBits = CreateCompatibleDC(hDC);
        GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);
        SelectObject(hDCBits, hBitmap);
        // Replace with StretchBlt call
        //bResult = BitBlt(hDC, x, y, Bitmap.bmWidth, Bitmap.bmHeight, hDCBits, 0, 0, dwROP);
        SetStretchBltMode(hDC, HALFTONE);
        bResult = StretchBlt(hDC, x, y, width, height,
            hDCBits, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, dwROP);
        DeleteDC(hDCBits);
        //::DeleteObject(hBitmap);

        return bResult;
    }

    static BYTE* GetPixelsFromHBITMAP(HBITMAP hBitmap)
    {
        HDC hdc = GetDC(0);

        BITMAPINFO MyBMInfo = { 0 };
        MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);

        // Get the BITMAPINFO structure from the bitmap
        if (0 == GetDIBits(hdc, hBitmap, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS)) {
            //cout << "error" << endl;
        }

        // create the bitmap buffer
        BYTE* lpPixels = new BYTE[MyBMInfo.bmiHeader.biSizeImage];

        // Better do this here - the original bitmap might have BI_BITFILEDS, which makes it
        // necessary to read the color table - you might not want this.
        MyBMInfo.bmiHeader.biCompression = BI_RGB;

        // get the actual bitmap buffer
        if (0 == GetDIBits(hdc, hBitmap, 0, MyBMInfo.bmiHeader.biHeight, (LPVOID)lpPixels, &MyBMInfo, DIB_RGB_COLORS)) {
            //cout << "error2" << endl;
        }

        return lpPixels;
    }

    static void CreateHBITMAPfromPixels(HBITMAP bitmap, BYTE* pImageData)
    {
        SetBitmapBits(bitmap, sizeof(pImageData), pImageData);
    }

    static HBITMAP HBITMAPFromPixels(const std::vector<uint8_t>& Pixels, int width, int height, int BitsPerPixel)
    {
        BITMAPINFO Info = { 0 };
        std::memset(&Info, 0, sizeof(BITMAPINFO));

        Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        Info.bmiHeader.biWidth = width;
        Info.bmiHeader.biHeight = height;
        Info.bmiHeader.biPlanes = 1;
        Info.bmiHeader.biBitCount = BitsPerPixel;
        Info.bmiHeader.biCompression = BI_RGB;
        Info.bmiHeader.biSizeImage = ((width * BitsPerPixel + 31) / 32) * 4 * height;

        HBITMAP Result = CreateDIBitmap(GetDC(nullptr), &Info.bmiHeader, CBM_INIT, &Pixels[0], &Info, DIB_RGB_COLORS);
        return Result;
    }

    static void HBITMAPToPixels(HBITMAP BitmapHandle, std::vector<uint8_t>& Pixels, int width, int height, int BitsPerPixel)
    {

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

