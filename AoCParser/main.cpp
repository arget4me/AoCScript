#include <string>
#include "Parser.h"

bool RunCode(std::string path, bool printSyntax = false)
{
	std::string code;
	if (!ReadFile(path, code))
	{
		return false;
	}

	PushConsoleColor(CONSOLE_COLOR::CYAN);
	std::cout << "Run code : " << path << "\n";
	PopConsoleColor();

	Parser parse(code, printSyntax);
	std::cout << "\n\n";
	return true;
}

void RunExample(std::string path) {
	try {
		RunCode(path, false);
	}
	catch (const std::invalid_argument& e) {
		(void)e; // Don't care about exceptions when running examples, since they also demonstrate asserts etc.
		std::cout << "\n\n";
	}
}

void RunExamples() {
	std::cout << "RUNNING EXAMPLES\n" << std::endl;
	
	RunExample("examples/example1.aoc");
	RunExample("examples/example2.aoc");
	RunExample("examples/example3.aoc");
	RunExample("examples/example4.aoc");
	RunExample("examples/example5.aoc");
	RunExample("examples/example6.aoc");
	RunExample("examples/example7.aoc");
	
	std::cout << "Examples DONE!\n" << std::endl;
}

void RunTest(std::string path, bool& setOnFail)
{
	try {
		RunCode("tests/aoc_day1_test.aoc");
	}
	catch (const std::invalid_argument& e) {
		(void)e; // Won't use the exception object since i should already be handled, will just display fail message.
		setOnFail = true;
	}
}

void RunTests() {
	bool testsFailed = false;
	std::cout << "RUNNING TESTS\n" << std::endl;
	RunTest("tests/aoc_day1_test.aoc", testsFailed);

	if (testsFailed) {
		PushConsoleColor(CONSOLE_COLOR::RED);
		std::cout << "Tests FAILED!\n" << std::endl;
		PopConsoleColor();
	}
	else {
		PushConsoleColor(CONSOLE_COLOR::GREEN);
		std::cout << "Tests SUCCESS!\n" << std::endl;
		PopConsoleColor();
	}
}

int main(int argc, char* argv[])
{
#if defined(_DEBUG)
	if (argc >= 2) {
		const int STR_EQUALS = 0;
		if (std::string(argv[1]).compare("--examples") == STR_EQUALS) {
			RunExamples();
		}
	}
	RunTests();
#else
	if (argc != 2) {
		PushConsoleColor(CONSOLE_COLOR::RED);
		std::cerr << "Must pass only one argument with the .aoc file to compile!" << std::endl;
		std::cerr << "Bad Arguments: ";
		for (int i = 0; i < argc; i++) {
			std::cerr << "'" << argv[i] << "' ";
		}
		std::cerr << std::endl;

		PopConsoleColor();
		return 1;
	}

	std::string aocSourceFile = argv[1];
	if (aocSourceFile.size() >= 4 && aocSourceFile.substr(aocSourceFile.size() - 4) == ".aoc") {
		if (!RunCode(aocSourceFile)) {
			PushConsoleColor(CONSOLE_COLOR::RED);
			std::cerr << "File not found: '" << aocSourceFile << "'" << std::endl;
			PopConsoleColor();
			return 2;
		}
	}
	else {
		PushConsoleColor(CONSOLE_COLOR::RED);
		std::cout << "The string does not end with .aoc" << std::endl;
		std::cerr << "Bad file argument: '" << aocSourceFile << "'" << std::endl;
		PopConsoleColor();
		return 3;
	}
#endif
	return 0;
}