#include <string>
#include "Parser.h"

#include <fstream>
#include <sstream>

bool ReadFile(const std::string& filePath, std::string& fileContents) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		return false;
	}

	// Read the entire file into a string
	std::ostringstream buffer;
	buffer << file.rdbuf();

	fileContents = buffer.str();

	file.close();
	return true;
}

void RunExample(std::string path, bool printSyntax = false)
{
	std::string code;
	if (!ReadFile(path, code))
	{
		return;
	}
	std::cout << "Run code : " << path << "\n";
	Parser parse(code, printSyntax);
	std::cout << "\n\n";
}

int main(int argc, char* argv[])
{
	RunExample("example1.aoc", false);
	RunExample("example2.aoc", false);
	RunExample("example3.aoc", false);
	RunExample("example4.aoc", false);
	return 0;
}