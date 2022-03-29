#pragma once
#include <windows.h>
class CreateUI
{
public:
    HWND static CreateTextBox(LPCWSTR text, INT xPos, INT yPos, INT width, INT height, HWND parrent);

    HWND static CreateButton(LPCWSTR text, INT xPos, INT yPos, INT width, INT height, HWND parrent, INT hMenuNum);

    HWND static CreateInputBox(LPCWSTR text, INT xPos, INT yPos, INT width, INT height, HWND parrent);
};

