#include <string>
#include "Parser.h"
int main(int argc, char* argv[])
{
	std::string code =	
						"load \"Day 0\";"
						"a = 1 + 2 - 2 + 45;"
						"c2a = 12 - a;"
						"a = a + c2a + 12 - a +1;"
						"print a;"
						"simon says c2a;";
	Parser parse(code);
	return 0;
}