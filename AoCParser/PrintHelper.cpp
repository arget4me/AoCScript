#include "PrintHelper.h"
#include <vector>
#include <string>
#include <iostream>

#if defined(_WIN32)
// To enable color output in windows CMD with ANSI escape code
#include <Windows.h>
#endif

static std::vector<CONSOLE_COLOR> ColorStack = {};

void InitializePrintHelper()
{
#if defined(_WIN32)
	// Enable virtual terminal processing (to support ANSI escape codes)
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
#endif
}

std::string ConsoleColorToString(CONSOLE_COLOR color) {
	const std::string WHITE = "\033[37m";
	const std::string RED = "\033[91m";
	const std::string GREEN = "\033[92m";
	const std::string YELLOW = "\033[93m";

	// Custom light blue using RGB (e.g., R=50, G=80, B=255) since "\033[94m" is very dark and unreadable
	const std::string BLUE = "\033[38;2;50;80;255m";
	const std::string MAGENTA = "\033[95m";
	const std::string CYAN = "\033[96m";
	const std::string RESET = "\033[0m";

	std::string result = RESET;
	switch (color) {
	case CONSOLE_COLOR::WHITE:
		result = WHITE;
		break;
	case CONSOLE_COLOR::RED:
		result = RED;
		break;
	case CONSOLE_COLOR::YELLOW:
		result = YELLOW;
		break;
	case CONSOLE_COLOR::GREEN:
		result = GREEN;
		break;
	case CONSOLE_COLOR::BLUE:
		result = BLUE;
		break;
	case CONSOLE_COLOR::MAGENTA:
		result = MAGENTA;
		break;
	case CONSOLE_COLOR::CYAN:
		result = CYAN;
		break;
	default:
		break;
	}
	return result;
}

void PushConsoleColor(CONSOLE_COLOR color) {
	ColorStack.push_back(color);
	std::cout << ConsoleColorToString(color);
}

void PopConsoleColor() {
	CONSOLE_COLOR color = CONSOLE_COLOR::RESET;
	if (ColorStack.size() > 0) {
		ColorStack.pop_back();
		if (ColorStack.size() != 0) {
			color = ColorStack.back();
		}
	}
	std::cout << ConsoleColorToString(color);
}

void ResetConsoleColor() {
	std::cout << ConsoleColorToString(CONSOLE_COLOR::RESET);
}