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
	Parser parse(code);
	return 0;
}