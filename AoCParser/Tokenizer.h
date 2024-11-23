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
	Token() : type(TokenType::END), value("") {}
	Token(TokenType type, std::string value) : type(type), value(value) {}
	TokenType type;
	std::string value;
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
	Tokenizer(std::string code) : code(code), cursor(code), nextToken(TokenType::END, "") { scanToken(); };
	~Tokenizer() = default;

	void print(Token token);

	Token GetNextToken() {
		Token result = nextToken;
		scanToken();
		return result;
	}
private:
	bool scanToken();
	//void scanToken();
	bool scanPRINT(int index);
	int scanNUM(int index, Token& outToken);
	int scanID(int index, Token& outToken);

	Token nextToken;
	std::string code;
	std::string cursor;
	//int head;
};
