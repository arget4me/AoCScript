#include <string>
#include "Parser.h"
int main(int argc, char* argv[])
{
	std::string code =	
	"load \"Day \n0\";"
	"a = 1 + 2 - 2 + 45;"
	"c2a = 3*12 - a*2;"
	"a = 4*a + c2a + 12 - a/3*5 +1;"
	"print a;"
	"simon says c2a;";

	std::string code2 = 
	"load \"Day 0\";"
	"a = 1 + 2 - 2 + 45;"
	"c2a = 3*12 - a*2;"
	"f = 0 + 0 - 0 + 0 * 2 / 1 * 3 + 1 * 3 - 3;"
	"print f;"
	"if f + 2 :"
	"g = 3;"
	"c2a = c2a + g;"
	"print g;"
	"else:"
	"fg = 13;"
	"c2a = 5 + fg + c2a;"
	"print fg;"
	"end;"
	"a = 4*a + c2a + 12 - a/3*5 +1;"
	"print a;"
	"simon says c2a;";


	Parser parse(code2);
	return 0;
}