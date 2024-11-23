#include "Parser.h"


Parser::Parser(std::string code) : tokenizer(code), ast(nullptr)
{
	Token t;
	while (tokenizer.GetNextToken(t) && t.type != TokenType::END)
	{
		tokenizer.print(t);
	}
}

Parser::~Parser()
{
	for (TreeNode* node : nodes)
	{
		delete node;
	}
}
