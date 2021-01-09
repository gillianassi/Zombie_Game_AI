#include "stdafx.h"
#include <iostream>
#include "ConsoleOutputReducer.h"

// Code from Nillo 
COORD GetConsoleCursorPosition()
{
	std::cout.flush();
	HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
	{
		return cbsi.dwCursorPosition;
	}
	// The function failed. Call GetLastError() for details.
	COORD invalid = { 0, 0 };
	return invalid;
}

void GoToConsoleCursorPosition(COORD pos)
{
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(output, pos);
	std::cout << "                                                                                   \r";
}