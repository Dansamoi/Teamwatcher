#include "UIElement.h"

UIElement::UIElement()
{
    // constructor of basic UIElement
    this->type = NOTHING;
    this->name = LPWSTR("");
    this->parentWindow = NULL;
    this->handle = NULL;
    this->hFont = NULL;
    this->xPos = 0;
    this->yPos = 0;
    this->width = 0;
    this->height = 0;
}

UIElement::UIElement(UIType type, LPWSTR name, HWND parentWindow)
{
    // different constructor of basic UIElement
    this->type = type;
    this->text = text;
    this->name = name;
    this->parentWindow = parentWindow;
    this->handle = NULL;
    this->xPos = 0;
    this->yPos = 0;
    this->width = 0;
    this->height = 0;
}

LPWSTR UIElement::getName()
{
    // name getter
    return name;
}

UIType UIElement::getType()
{
    // type getter
    return type;
}

HWND UIElement::getParent()
{
    // window parent getter
    return parentWindow;
}

HWND UIElement::getHandle()
{
    // handle getter
    return this->handle;
}

BOOL UIElement::getText(wchar_t* buff, INT count)
{
    // text getter
    return GetWindowText(handle, buff, count);
}

BOOL UIElement::setText(LPWSTR text)
{
    // text setter
    this->text = text;
    return SetWindowText(handle, text);
}

void UIElement::setFont(INT size, INT weight, LPWSTR fontName)
{
    // font setter
    if(this->hFont != NULL)
        DeleteObject(this->hFont);
    this->hFont = CreateFont(
        size,
        0, 0, 0,
        weight,
        FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        fontName);

    SendMessage(handle, WM_SETFONT, WPARAM(this->hFont), TRUE);
}

void UIElement::show()
{
    // show element
    ShowWindow(handle, SW_SHOW);
}

void UIElement::hide()
{
    // hide element
    ShowWindow(handle, SW_HIDE);
}
