#pragma once
#ifndef UIMANAGER_H
#define UIMANAGER_H
#include <map>
#include <vector>
#include <windows.h>
#include "UIElement.h"
#include "Label.h"
#include "Button.h"
#include "TextBox.h"

class UIManager
{
	// Class of all the UI Managment

private:
	// Handle to window
	HWND window;

	// map of menus and vector that saves UIElements
	std::map<INT, std::vector<UIElement*>> Menus;

	// vector of all taken Menus
	std::vector<INT> MenuNumbers;
	
	// function to check if element is in a menu
	BOOL menuContainName(INT menuNum, LPWSTR name);
public:
	// constructor
	UIManager(HWND window);

	// destructor 
	~UIManager();

	// add menu to menus 
	void AddMenu(INT menuNum);

	// add element to menu
	void AddElement(INT menuNum, UIType Type, LPWSTR name, LPWSTR text, INT xPos, INT yPos, INT width, INT height, INT hMenu);

	// remove element from menu
	void removeElement(INT menuNum, LPWSTR name);

	// get element from menu
	UIElement* getElementByName(INT menuNum, LPWSTR name);

	// show specific menu
	void showMenu(INT menuNum);

	// hide specific menu
	void hideMenu(INT menuNum);

	// moves from one screen to other
	void moveTo(INT menuNumFrom, INT menuNumTo);
};

#endif // UIMANAGER_H