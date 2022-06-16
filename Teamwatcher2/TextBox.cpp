#include "TextBox.h"

TextBox::TextBox(LPWSTR text, LPWSTR name, INT xPos, INT yPos, INT width, INT height, HWND parentWindow)
{
    // TexBox UIElement constructor

    this->text = text;
    this->type = TEXT_BOX;
    this->name = name;
    this->parentWindow = parentWindow;
    this->xPos = xPos;
    this->yPos = yPos;
    this->width = width;
    this->height = height;
    this->hFont = NULL;
    this->handle = CreateWindow(
        L"EDIT",
        text,
        WS_VISIBLE | WS_CHILD | WS_VISIBLE | WS_BORDER,
        xPos, yPos, width, height, parentWindow,
        NULL, NULL, NULL);
}
