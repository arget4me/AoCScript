#pragma once
#include <string>

enum class CONSOLE_COLOR {
	WHITE,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	RESET,
};

std::string ConsoleColorToString(CONSOLE_COLOR color);
void PushConsoleColor(CONSOLE_COLOR color);
void PopConsoleColor();
void ResetConsoleColor();

void InitializePrintHelper();