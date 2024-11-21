#pragma once
#include <string>
#include <iostream>

enum class TokenType
{
	INTEGER,
	PLUS,
	MINUS,
	ID,
	SEMICOLON,
	EQUALS,
	PRINT,
	END
};

class Token
{
public:
	TokenType type;
	int value_int;
	std::string value_str;
};

class IntegerToken : public Token
{
public:
};

class IDToken : public Token
{
public:
};

class Tokenizer
{
public:
	Tokenizer(std::string code) : code(code), head(0) { scanToken(); };
	~Tokenizer() = default;

	void print(Token token);

	Token GetNextToken() {
		Token result = nextToken;
		scanToken();
		return result;
	}
private:
	void scanToken();
	bool scanPRINT(int index);
	int scanNUM(int index, Token& outToken);
	int scanID(int index, Token& outToken);

	Token nextToken;
	std::string code;
	int head;
};
