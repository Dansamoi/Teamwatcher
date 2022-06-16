#pragma once
#ifndef UIELEMENT_H
#define UIELEMENT_H
#include <windows.h>

// enum of different UI elements Types
enum UIType
{
	LABEL,
	BUTTON,
	TEXT_BOX,
	NOTHING
};

class UIElement
{
	// Class of basic UIElement
protected:
	UIType type; // UI element Type
	LPWSTR name; // name of element
	LPWSTR text; // text on element
	HFONT hFont; // font of element
	HWND parentWindow; // the window the element in
	HWND handle; // handle to the element
	INT xPos; // x position
	INT yPos; // y position
	INT width; // width of element
	INT height; // height of element
public:
	// constructor
	UIElement();

	// destructor
	UIElement(UIType type, LPWSTR name, HWND parentWindow);

	// name getter
	LPWSTR getName();

	// type getter
	UIType getType();

	// parent window getter
	HWND getParent();

	// element handle getter
	HWND getHandle();

	// text getter
	BOOL getText(wchar_t* buff, INT count);

	// text setter
	BOOL setText(LPWSTR text);

	// font setter
	void setFont(INT size, INT weight, LPWSTR fontName);

	// show element
	void show();

	// hide element
	void hide();
};

#endif // UIELEMENT_H