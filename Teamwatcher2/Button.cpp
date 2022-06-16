#include "Button.h"

Button::Button(LPWSTR text, LPWSTR name, INT xPos, INT yPos, INT width, INT height, HWND parentWindow, HMENU msg)
{
    // Button UIElement constructor
    this->type = BUTTON;
    this->text = text;
    this->name = name;
    this->parentWindow = parentWindow;
    this->xPos = xPos;
    this->yPos = yPos;
    this->width = width;
    this->height = height;
    this->msg = msg;
    this->hFont = NULL;
    this->handle = CreateWindow(
        L"BUTTON",
        text,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        xPos, yPos, width, height, parentWindow,
        msg, NULL, NULL);
}
