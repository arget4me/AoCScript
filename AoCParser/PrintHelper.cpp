#include "PrintHelper.h"
#include <vector>
#include <string>
#include <iostream>

static std::vector<CONSOLE_COLOR> ColorStack = {};
static std::string ToString(CONSOLE_COLOR color) {
	const std::string WHITE = "\033[37m";
	const std::string RED = "\033[91m";
	const std::string GREEN = "\033[92m";
	const std::string YELLOW = "\033[93m";
	const std::string BLUE = "\033[94m";
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
	std::cout << ToString(color);
}

void PopConsoleColor() {
	CONSOLE_COLOR color = CONSOLE_COLOR::RESET;
	if (ColorStack.size() > 0) {
		ColorStack.pop_back();
		if (ColorStack.size() != 0) {
			color = ColorStack.back();
		}
	}
	std::cout << ToString(color);
}