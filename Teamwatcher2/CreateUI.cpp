#include "CreateUI.h"

HWND CreateUI::CreateTextBox(LPCWSTR text, INT xPos, INT yPos, INT width, INT height, HWND parrent)
{
    HWND hwnd = CreateWindow(
        L"STATIC",
        text,
        WS_VISIBLE | WS_CHILD,
        xPos, yPos, width, height, parrent,
        NULL, NULL, NULL);
    return hwnd;
}

HWND CreateUI::CreateButton(LPCWSTR text, INT xPos, INT yPos, INT width, INT height, HWND parrent, INT hMenuNum)
{
    HWND hwnd = CreateWindow(
        L"BUTTON",
        text,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        xPos, yPos, width, height, parrent,
        (HMENU)hMenuNum, NULL, NULL);
    return hwnd;
}

HWND CreateUI::CreateInputBox(LPCWSTR text, INT xPos, INT yPos, INT width, INT height, HWND parrent)
{
    HWND hwnd = CreateWindow(
        L"EDIT",
        text,
        WS_VISIBLE | WS_CHILD | WS_VISIBLE | WS_BORDER,
        xPos, yPos, width, height, parrent,
        NULL, NULL, NULL);
    return hwnd;
}


