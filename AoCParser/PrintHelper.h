#pragma once


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

void PushConsoleColor(CONSOLE_COLOR color);
void PopConsoleColor();