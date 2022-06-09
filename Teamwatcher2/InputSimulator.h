#pragma once
#include <windows.h>

class InputSimulator
{
public:
	// Simulate Mouse Input
	static void SimulateMouseInput(int input, int x, int y);

	// Simulate Keyboard Input
	static void SimulateKeyInput(int inputKey, int flag);
};

