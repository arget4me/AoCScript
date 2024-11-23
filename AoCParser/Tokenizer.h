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

	std::string ToString()
	{
		std::string type_string = "";
		switch (type)
		{
			case TokenType::INTEGER:			{ type_string = "INTEGER";	 }	break;
			case TokenType::PLUS:				{ type_string = "PLUS";		 }	break;
			case TokenType::MINUS:				{ type_string = "MINUS";	 }	break;
			case TokenType::ID:					{ type_string = "ID";		 }	break;
			case TokenType::SEMICOLON:			{ type_string = "SEMICOLON"; }	break;
			case TokenType::EQUALS:				{ type_string = "EQUALS";	 }	break;
			case TokenType::PRINT:				{ type_string = "PRINT";	 }	break;
			case TokenType::LOAD:				{ type_string = "LOAD";		 }	break;
			case TokenType::STRING:				{ type_string = "STRING";	 }	break;
			case TokenType::END:				{ type_string = "END";		 }	break;
			default: break;
		}
		std::string result = "{" + type_string + " : " + value + "}";
		return result;
	}
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

	/* Some time the next token is needed without consuming it, then Peeking is usable.
	   Must manully call ConsumeNext to consume it or call GetNextToken again.. */
	bool PeekNextToken(Token& outToken) {
		std::string saveCursor = cursor;
		Token saveToken = nextToken;
		bool result = scanToken();
		outToken = nextToken;
		nextToken = saveToken;
		cursor = saveCursor;
		return result;
	}

	bool ConsumeNext() {
		return scanToken();
	}

private:
	bool scanToken();
	bool scanPRINT(int index);
	int scanNUM(int index, Token& outToken);
	int scanID(int index, Token& outToken);

	Token nextToken;
	std::string code;
	std::string cursor;
	//int head;
};
