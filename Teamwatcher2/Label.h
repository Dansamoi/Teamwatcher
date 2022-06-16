#pragma once
#ifndef LABEL_H
#define LABEL_H
#include "UIElement.h"

class Label : public virtual UIElement
{
	// Class Label UIElement

public:
	// Label constructor
	Label(LPWSTR text, LPWSTR name, INT xPos, INT yPos, INT width, INT height, HWND parentWindow);
};

#endif // LABEL_H