#include <iostream>
#include <string>

#include "Parser.h"



int main(int argc, char* argv[])
{
	std::string code =	"a = 1 + 2;\n"
						"print a;";
	Parser parse(code);
	return 0;
}