#pragma once
#include <windows.h>

class InputSimulator
{
public:
	static void SimulateMouseInput(int input, int x, int y);

	static void SimulateKeyInput(int inputKey, int flag);
};

