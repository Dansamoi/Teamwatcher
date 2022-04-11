#pragma once
#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <windows.h>

class Clipboard
{
public:
	static void toClipboard(HWND hwnd, LPWSTR s);
};

#endif // CLIPBOARD_H