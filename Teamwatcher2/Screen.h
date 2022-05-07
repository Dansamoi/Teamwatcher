#ifndef SCREEN
#define SCREEN

#pragma once
#include <windows.h>

class Screen
{
public:
	static HBITMAP GetScreenShot(void)
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
        //HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
        HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
       // BOOL    bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
        BOOL    bRet = BitBlt(hDC, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), hScreen, 0, 0, SRCCOPY);

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
        HDC       hDCBits;
        BITMAP    Bitmap;
        BOOL      bResult;

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
        ::DeleteObject(hBitmap);

        return bResult;
    }
};

#endif // SCREEN

