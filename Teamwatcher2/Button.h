#pragma once
#ifndef BUTTON_H
#define BUTTON_H
#include "UIElement.h"

class Button : public virtual UIElement
{
	// Class Button UIElement
private:
	HMENU msg; // msg to Async msg system
public:
	// Button element constructor
	Button(LPWSTR text, LPWSTR name, INT xPos, INT yPos, INT width, INT height, HWND parentWindow, HMENU msg);
};

#endif // BUTTON_H