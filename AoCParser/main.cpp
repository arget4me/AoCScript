#include <iostream>
#include <string>

#include "Tokenizer.h"



int main(int argc, char* argv[])
{
	std::cout << "\n\nHello, World" << std::endl << std::endl;
	std::string code =	"a = 1 + 2;\n"
						"print a;";

	Tokenizer tokenizer(code);
	Token t;
	do {
		t = tokenizer.GetNextToken();
		tokenizer.print(t);
	} while (t.type != TokenType::END);

	std::cout << "\n\nHello, World" << std::endl;
	return 0;
}