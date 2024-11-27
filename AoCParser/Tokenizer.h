#pragma once
#include <string>
#include <iostream>
#include <regex>
#include <vector>

enum class TokenType
{
	INTEGER,
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	ID,
	SEMICOLON,
	EQUALS,
	PRINT,
	LOAD,
	STRING,
	MODULO,

	// IfStatement
	IF, // "if"
	IF_ELSE, // "else"
	IF_CLOSE, // "end"
	COLON, // ":"

	// LoopStatement
	LOOP,
	LOOP_TIMES,
	LOOP_STOP,

	END,
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
			case TokenType::MULTIPLY:			{ type_string = "MULTIPLY";	 }	break;
			case TokenType::DIVIDE:				{ type_string = "DIVIDE";	 }	break;
			case TokenType::ID:					{ type_string = "ID";		 }	break;
			case TokenType::SEMICOLON:			{ type_string = "SEMICOLON"; }	break;
			case TokenType::EQUALS:				{ type_string = "EQUALS";	 }	break;
			case TokenType::PRINT:				{ type_string = "PRINT";	 }	break;
			case TokenType::LOAD:				{ type_string = "LOAD";		 }	break;
			case TokenType::STRING:				{ type_string = "STRING";	 }	break;
			
			// IfStatement
			case TokenType::IF:				{ type_string = "IF";	 }	break;
			case TokenType::IF_ELSE:				{ type_string = "IF_ELSE";	 }	break;
			case TokenType::IF_CLOSE:				{ type_string = "IF_CLOSE";	 }	break;
			case TokenType::COLON:				{ type_string = "COLON";	 }	break;

			case TokenType::END:				{ type_string = "END";		 }	break;
			default: break;
		}
		std::string result = "{" + type_string + " : " + value + "}";
		return result;
	}
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

	Token nextToken;
	std::string code;
	std::string cursor;

	bool checkTokenMap(const std::vector<std::pair<std::regex, TokenType>>& tokenMap, std::string& line);
};
