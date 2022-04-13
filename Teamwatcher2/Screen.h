#ifndef SCREEN
#define SCREEN

#pragma once
#include <windows.h>
#include <iostream>
#include <functional>
#include <vector>

class Screen
{
public:
	static void GetScreenShot(void)
    {
        int x, y, w, h;

        // get screen dimensions
        x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        y = GetSystemMetrics(SM_YVIRTUALSCREEN);
        w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        // copy screen to bitmap
        HDC     hScreen = GetDC(NULL);
        HDC     hDC = CreateCompatibleDC(hScreen);
        HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
        HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
        BOOL    bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);

        // save bitmap to clipboard
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

    /*
    std::unique_ptr<std::remove_pointer<HBITMAP>::type, std::function<void(HBITMAP)>> HBITMAPFromPixels(const std::vector<std::uint8_t>& Pixels, std::uint32_t width, std::int32_t height, std::uint16_t BitsPerPixel)
    {
        BITMAPINFO Info = { 0 };
        std::memset(&Info, 0, sizeof(BITMAPINFO));

        Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        Info.bmiHeader.biWidth = width;
        Info.bmiHeader.biHeight = -height;
        Info.bmiHeader.biPlanes = 1;
        Info.bmiHeader.biBitCount = BitsPerPixel;
        Info.bmiHeader.biCompression = BI_RGB;
        Info.bmiHeader.biSizeImage = ((width * BitsPerPixel + 31) / 32) * 4 * height;

        HBITMAP Result = CreateDIBitmap(GetDC(nullptr), &Info.bmiHeader, CBM_INIT, &Pixels[0], &Info, DIB_RGB_COLORS);
        return std::unique_ptr<std::remove_pointer<HBITMAP>::type, std::function<void(HBITMAP)>>(Result, [&](HBITMAP hBmp) {DeleteObject(hBmp); });
    }

    void HBITMAPToPixels(HBITMAP BitmapHandle, std::vector<std::uint8_t>& Pixels, std::uint32_t& width, std::int32_t& height, std::uint16_t& BitsPerPixel)
    {
        if (BitmapHandle == nullptr)
        {
            throw std::logic_error("Null Pointer Exception. BitmapHandle is Null.");
        }

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
    */
};

#endif // SCREEN

