#include "UIManager.h"

BOOL UIManager::menuContainName(INT menuNum, LPWSTR name)
{
	// The function checks if specific menu contain element
	// that has the name = name
	for (auto element : Menus[menuNum])
	{
		if (wcscmp(element->getName(), name) == 0)
			return TRUE;
	}
	return FALSE;
}

UIManager::UIManager(HWND window)
{
	// constructor for UIManager
	this->window = window;
}

UIManager::~UIManager()
{
	// destructor of UIManager
	for (auto menu : Menus) {
		for (auto element : menu.second) {
			delete element;
		}
	}
}

void UIManager::AddMenu(INT menuNum)
{
	// Adds menu to Menus of UIManager

	// checks if menu not already created
	if (!count(MenuNumbers.begin(), MenuNumbers.end(), menuNum)) {
		MenuNumbers.push_back(menuNum);
		Menus[menuNum] = std::vector<UIElement*>();
	}
}

void UIManager::AddElement(INT menuNum, UIType Type, LPWSTR name, LPWSTR text, INT xPos, INT yPos, INT width, INT height, INT hMenu)
{
	// Adds menu to Menus of UIManager

	// checks if element name not already taken
	if (!menuContainName(menuNum, name)) {

		// checks if menu created
		if (count(MenuNumbers.begin(), MenuNumbers.end(), menuNum)) {
			UIElement* tmpElement;

			// create the element according to type
			switch (Type) {
			case LABEL:
				// create label
				tmpElement = new Label(
					text, 
					name, 
					xPos, 
					yPos, 
					width, 
					height, 
					window);
				break;

			case BUTTON:
				// create button
				tmpElement = new Button(
					text, 
					name, 
					xPos, 
					yPos, 
					width, 
					height, 
					window, 
					(HMENU)hMenu);
				break;

			case TEXT_BOX:
				// create TextBox
				tmpElement = new TextBox(
					text, 
					name, 
					xPos, 
					yPos, 
					width, 
					height, 
					window);
				break;
			}
			Menus[menuNum].push_back(tmpElement);
		}
	}
}

void UIManager::removeElement(INT menuNum, LPWSTR name)
{
	// remove element from specific menu of UIManager
	int counter = 0;

	// finds element position in vector
	for (auto element : Menus[menuNum]) {
		if (wcscmp(element->getName(), name) == 0)
			break;
		counter++;
	}

	// if position in the vector - erase
	if (!(counter >= Menus[menuNum].size())) {
		delete Menus[menuNum][counter];
		Menus[menuNum].erase(Menus[menuNum].begin() + counter);
	}
}

UIElement* UIManager::getElementByName(INT menuNum, LPWSTR name)
{
	// get element from specific menu of UIManager
	int counter = 0;

	// finds element position in vector
	for (auto element : Menus[menuNum]) {
		if (wcscmp(element->getName(), name) == 0)
			break;
		counter++;
	}

	// if position in the vector - returns
	if (!(counter >= Menus[menuNum].size()))
		return Menus[menuNum][counter];

	// if not fount, returns NULL
	return NULL;
}

void UIManager::showMenu(INT menuNum)
{
	// Update all the elements in specific menu to SW_SHOW
	// which make the elements appear
	for (auto element : Menus[menuNum]) {
		element->show();
	}
}

void UIManager::hideMenu(INT menuNum)
{
	// Update all the elements in specific menu to SW_HIDE
	// which hides the elements
	for (auto element : Menus[menuNum]) {
		element->hide();
	}
}

void UIManager::moveTo(INT menuNumFrom, INT menuNumTo)
{
	// Move from one menu to another
	hideMenu(menuNumFrom);
	showMenu(menuNumTo);
}
