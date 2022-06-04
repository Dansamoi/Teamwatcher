#include "InputSimulator.h"

void InputSimulator::SimulateMouseInput(int input, int x, int y)
{
	INPUT Inputs[2] = { 0 };

	// Set up a generic mouse event

	// set up mouse move event
	Inputs[0].type = INPUT_MOUSE;
	Inputs[0].mi.dx = x; // desired X coordinate
	Inputs[0].mi.dy = y; // desired Y coordinate
	Inputs[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	// set up mouse event
	Inputs[1].type = INPUT_MOUSE;
	Inputs[1].mi.dwFlags = input;

	//Inputs[1].type = INPUT_MOUSE;
	//Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	//Inputs[2].type = INPUT_MOUSE;
	//Inputs[2].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	SendInput(2, Inputs, sizeof(INPUT));
}

void InputSimulator::SimulateKeyInput(int inputKey, int flag)
{
	INPUT Input = { 0 };

	// Set up a generic keyboard event.
	Input.type = INPUT_KEYBOARD;
	Input.ki.wScan = 0; // hardware scan code for key
	Input.ki.time = 0;
	Input.ki.dwExtraInfo = 0;

	// Press the key
	Input.ki.wVk = inputKey; // virtual-key code for key
	Input.ki.dwFlags = flag; // 0 for key press, 2 for key up

	SendInput(1, &Input, sizeof(INPUT));
}
