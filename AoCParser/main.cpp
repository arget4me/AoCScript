#include <string>
#include "Parser.h"
int main(int argc, char* argv[])
{
	std::string code =	"a = 1 + 2;\n"
						"c2a = 12 - a;\n"
						"print a;"
						"print c2a;";
	Parser parse(code);
	return 0;
}