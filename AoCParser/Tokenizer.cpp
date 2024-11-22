#include "Tokenizer.h"
#include <cctype> // For std::isdigit

bool Tokenizer::scanPRINT(int index)
{
	if (code.length() - index <= 5) return false;
	const char print[6] = "print";
	for (int i = 0; i < 5; i++)
	{
		int nextIndex = index + i;
		if (nextIndex >= code.length()) return false;
		if (code[nextIndex] != print[i]) return false;
	}
	return true;
}

int Tokenizer::scanNUM(int index, Token& outToken) {
	int rightmost = -1;
	for (int i = index; i < code.length(); i++) {
		if (!std::isdigit(code[i])) break;
		rightmost = i;
	}

	std::string substring = code.substr(index, rightmost - index + 1);
	int number = 0;
	for (char c : substring) {
		number = number * 10;
		number = number + (c - '0'); // Convert char to digit
	}

	outToken.type = TokenType::INTEGER;
	outToken.value_int = number;
	return rightmost + 1;
}

int Tokenizer::scanID(int index, Token& outToken)
{
	int rightmost = -1;
	for (int i = index; i < code.length(); i++) {
		if (i == index && !std::isalpha(code[i])) return index; // ID must start with a character not a number
		if (!(std::isdigit(code[i]) || std::isalpha(code[i]))) break;
		rightmost = i;
	}

	std::string substring = code.substr(index, rightmost - index + 1);
	outToken.type = TokenType::ID;
	outToken.value_str = substring;
	return rightmost + 1;
}

void Tokenizer::scanToken()
{
	Token foundToken = {}; 
	foundToken.type = TokenType::END;
	for (int i = head; i < code.length(); i++)
	{
		if (code[i] == '+') {
			foundToken.type = TokenType::PLUS;
			head = i + 1;
			break;
		}
		if (code[i] == '-') {
			foundToken.type = TokenType::MINUS;
			head = i + 1;
			break;
		}
		if (code[i] == '=') {
			foundToken.type = TokenType::EQUALS;
			head = i + 1;
			break;
		}
		if (code[i] == ';') {
			foundToken.type = TokenType::SEMICOLON;
			head = i + 1;
			break;
		}
		if (code[i] == 'p' && scanPRINT(i)) {
			foundToken.type = TokenType::PRINT;
			head = i + 5;
			break;
		}
		if (std::isdigit(code[i])) {
			head = scanNUM(i, foundToken); // Sets TokenType::INTEGER
			break;
		}
		if (std::isalpha(code[i])) {
			head = scanID(i, foundToken); // Sets TokenType::INTEGER
			break;
		}
		head++;
	}
	nextToken = foundToken;
}

void Tokenizer::print(Token token)
{
	switch (token.type)
	{
		case TokenType::INTEGER:
			std::cout << "INTEGER:" << token.value_int << " ";
			break;
		case TokenType::PLUS:
			std::cout << "PLUS" << " ";
			break;
		case TokenType::MINUS:
			std::cout << "MINUS" << " ";
			break;
		case TokenType::ID:
			std::cout << "ID:" << token.value_str << " ";
			break;
		case TokenType::SEMICOLON:
			std::cout << "SEMICOLON\n" << " ";
			break;
		case TokenType::EQUALS:
			std::cout << "EQUALS" << " ";
			break;
		case TokenType::PRINT:
			std::cout << "PRINT"<< " ";
			break;
		case TokenType::END:
			std::cout << "END\n";
			break;
	}

}



