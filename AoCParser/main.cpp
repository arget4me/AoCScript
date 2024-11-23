#include <string>
#include "Parser.h"
int main(int argc, char* argv[])
{
	std::string code =	
						"load \"Day 0\";"
						"a = 1 + 2;"
						"c2a = 12 - a;"
						"print a;"
						"simon says c2a;";
	Parser parse(code);
	return 0;
}