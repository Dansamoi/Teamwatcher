#pragma once
#ifndef TEXTBOX_H
#define TEXTBOX_H
#include "UIElement.h"
class TextBox : public virtual UIElement
{
	// Class TexBox UIElement
public:
	// TexBox UIElement constructor
	TextBox(LPWSTR text, LPWSTR name, INT xPos, INT yPos, INT width, INT height, HWND parentWindow);
};

#endif // TEXTBOX_H