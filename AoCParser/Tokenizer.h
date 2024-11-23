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
	LOAD,
	STRING,
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
	Tokenizer(std::string code) : code(code), cursor(code), nextToken(TokenType::END, "") {};
	~Tokenizer() = default;

	void print(Token token);

	bool GetNextToken(Token& outToken) {
		bool result = scanToken();
		outToken = nextToken;
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
