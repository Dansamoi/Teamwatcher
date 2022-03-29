#pragma once
#include <Windows.h>

class SG_InputBox
{
public:
    static LPWSTR GetString(LPCTSTR szCaption, LPCTSTR szPrompt, LPCTSTR szDefaultText = L"");
};