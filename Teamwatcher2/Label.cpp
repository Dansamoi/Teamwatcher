#include "Label.h"

Label::Label(LPWSTR text, LPWSTR name, INT xPos, INT yPos, INT width, INT height, HWND parentWindow)
{
    // Label constructor

    this->type = LABEL;
    this->text = text;
    this->name = name;
    this->parentWindow = parentWindow;
    this->xPos = xPos;
    this->yPos = yPos;
    this->width = width;
    this->height = height;
    this->hFont = NULL;
    this->handle = CreateWindow(
        L"STATIC",
        text,
        WS_VISIBLE | WS_CHILD,
        xPos, yPos, width, height, parentWindow,
        NULL, NULL, NULL);
}