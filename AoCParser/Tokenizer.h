#pragma once
#include <string>
#include <iostream>
#include <regex>
#include <vector>

enum class TokenType
{
	SEMICOLON,	// ';'
	
	// Operators
	PLUS,		// '+'
	MINUS,		// '-'
	MULTIPLY,	// '*'
	DIVIDE,		// '/'
	EQUALS,		// '='
	MODULO,		// 'modulo'

	// Intrinsics
	PRINT,		// 'print' | 'simon says'
	LOAD,		// 'load'
	DAY,		// 'DAY'

	// Variable types
	INTEGER,
	STRING,
	ID,

	// Logic Operators
	GREATER_THAN,	// '>'
	GREATER_EQUALS,	// '>='
	LESS_THAN,		// '<'
	LESS_EQUALS,	// '<='
	IS_EQUAL,		// '=='
	IS_DIGIT,		// 'is DIGIT'
	IS_ALPHA,		// 'is ALPHA'

	// IfStatement
	IF,			// "if"
	IF_ELSE,	// "else"
	IF_CLOSE,	// "end"
	COLON,		// ":"

	// LoopStatement
	LOOP,		// 'loop'
	LOOP_TIMES, // 'times'
	LOOP_STOP,	// 'loopstop'
	LINE, // 'LINE'
	CHAR, // 'CHAR'
	LOOP_LINES, // 'lines'
	LOOP_CHARS, // 'chars'

	// AssertStatement
	ASSERT,		// 'assert'


	// Expressions
	LPAREN,		// '('
	RPAREN,		// ')'
	LBRACKET,	// '['
	RBRACKET,	// ']'
	LBRACE,		// '{'
	RBRACE,		// '}'
	ARRAY_SIZE,		// 'size'

	// Cast
	CAST_AS,		// 'as'
	TYPE_INTEGER,	// 'INTEGER'
	TYPE_STRING,	// 'STRING'
	TYPE_FLOAT,		// 'FLOAT'

	// List Assignment and Operators
	LIST,			// 'list'
	LIST_SORTED,	// 'sorted'
	LIST_UNSORTED,	// 'unsorted'
	LIST_ADD,		// '<<'

	



	END, // end of file stream (not really needed could be removed if wanted). Helps debugging
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
			case TokenType::MODULO:				{ type_string = "MODULO";	 }	break;

			// Logic Operators
			case TokenType::GREATER_THAN: { type_string = "GREATER_THAN";	 }	break;
			case TokenType::GREATER_EQUALS: { type_string = "GREATER_EQUALS";	 }	break;
			case TokenType::LESS_THAN: { type_string = "LESS_THAN";		 }	break;
			case TokenType::LESS_EQUALS: { type_string = "LESS_EQUALS";	 }	break;
			case TokenType::IS_EQUAL: { type_string = "IS_EQUAL";	 }	break;

			// IfStatement
			case TokenType::IF:				{ type_string = "IF";	 }	break;
			case TokenType::IF_ELSE:				{ type_string = "IF_ELSE";	 }	break;
			case TokenType::IF_CLOSE:				{ type_string = "IF_CLOSE";	 }	break;
			case TokenType::COLON:				{ type_string = "COLON";	 }	break;

			// LoopStatement
			case TokenType::LOOP: { type_string = "LOOP";	 }	break;
			case TokenType::LOOP_TIMES: { type_string = "LOOP_TIMES";	 }	break;
			case TokenType::LOOP_STOP: { type_string = "LOOP_STOP";	 }	break;
			case TokenType::LINE: { type_string = "LINE";	 }	break;
			case TokenType::CHAR: { type_string = "CHAR";	 }	break;
			case TokenType::LOOP_LINES: { type_string = "LOOP_LINES";	 }	break;
			case TokenType::LOOP_CHARS: { type_string = "LOOP_CHARS";	 }	break;

			// AssertStatement
			case TokenType::ASSERT: { type_string = "ASSERT";	 }	break;

			// DAY
			case TokenType::DAY: { type_string = "DAY";	 }	break;


			// CAST
			case TokenType::CAST_AS: { type_string = "CAST_AS";	 }	break;
			case TokenType::TYPE_INTEGER: { type_string = "TYPE_INTEGER";	 }	break;
			case TokenType::TYPE_STRING: { type_string = "TYPE_STRING";	 }	break;
			case TokenType::TYPE_FLOAT: { type_string = "TYPE_FLOAT";	 }	break;

			// LIST
			case TokenType::LIST: { type_string = "LIST";	 }	break;
			case TokenType::LIST_ADD: { type_string = "LIST_ADD";	 }	break;
			case TokenType::LIST_SORTED: { type_string = "LIST_SORTED";	 }	break;
			case TokenType::LIST_UNSORTED: { type_string = "LIST_UNSORTED";	 }	break;

			case TokenType::END:				{ type_string = "END";		 }	break;
			default: { type_string = "UNIMPLEMENTED: Token::ToString (" + std::to_string(static_cast<int>(type));		 }break;
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

	std::string GetLastLine() {
		return lastLine;
	}

private:
	bool scanToken();

	Token nextToken;
	std::string code;
	std::string cursor;
	std::string lastLine;

	bool checkTokenMap(const std::vector<std::pair<std::regex, TokenType>>& tokenMap, std::string& line);
};
