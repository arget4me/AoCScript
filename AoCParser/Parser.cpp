#include "Parser.h"


Parser::Parser(std::string code) : tokenizer(code), ast(nullptr)
{
	Token t;
	do {
		t = tokenizer.GetNextToken();
		tokenizer.print(t);
	} while (t.type != TokenType::END);
}

Parser::~Parser()
{
	for (TreeNode* node : nodes)
	{
		delete node;
	}
}
