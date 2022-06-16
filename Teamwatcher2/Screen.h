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

class Screen
{
    // Class for all screen data functionality
public:
    // This function is to get screenshot of the Screen, returns HBITMAP 
    static HBITMAP GetScreenShot(void);

    // Returns a resized HBITMAP
    static HBITMAP ResizeImage(HBITMAP hBitmap, int w, int h);
    
    // Drawing HBITMAP on screen
    static BOOL DrawBitmap(HDC hDC, INT x, INT y, INT width, INT height, HBITMAP hBitmap, DWORD dwROP);

    // Function returns a vector of bytes of HBITMAP pixels
    static HBITMAP HBITMAPFromPixels(const std::vector<uint8_t>& Pixels, int width, int height, int BitsPerPixel);

    // Function returns a HBITMAP from vector of bytes of pixels
    static void HBITMAPToPixels(HBITMAP BitmapHandle, std::vector<uint8_t>& Pixels, int width, int height, int BitsPerPixel);
};

#endif // SCREEN

