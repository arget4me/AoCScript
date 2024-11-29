#include <string>
#include "Parser.h"

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
	/*RunExample("example1.aoc", false);
	RunExample("example2.aoc", false);
	RunExample("example3.aoc", false);
	RunExample("example4.aoc", false);
	RunExample("example5.aoc", false);
	RunExample("example6.aoc", false);*/
	RunExample("example7.aoc", false);
	return 0;
}