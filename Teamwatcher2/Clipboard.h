#pragma once
#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <windows.h>

class Clipboard
	// Class for handling clipboard saving
{
public:
	// Save data to clipboard
	static void toClipboard(HWND hwnd, LPWSTR s);
};

#endif // CLIPBOARD_H