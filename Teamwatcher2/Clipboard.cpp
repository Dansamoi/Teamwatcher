#include "Clipboard.h"

void Clipboard::toClipboard(HWND hwnd, LPWSTR s)
{
	OpenClipboard(hwnd);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (wcslen(s) + 1) * sizeof(wchar_t));
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s, (wcslen(s) + 1) * sizeof(wchar_t));
	GlobalUnlock(hg);
	SetClipboardData(CF_UNICODETEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}
