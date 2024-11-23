#include "Tokenizer.h"
#include <regex>
#include <vector>

bool Tokenizer::scanToken()
{
	static const std::vector<std::pair<std::regex, TokenType>> tokenMap = {
		std::pair<std::regex, TokenType>{std::regex(R"(^simon says\b|^print\b)")			, TokenType::PRINT},
		std::pair<std::regex, TokenType>{std::regex(R"(^\+)")				, TokenType::PLUS},
		std::pair<std::regex, TokenType>{std::regex(R"(^-)")				, TokenType::MINUS},
		std::pair<std::regex, TokenType>{std::regex(R"(^=)")				, TokenType::EQUALS},
		std::pair<std::regex, TokenType>{std::regex(R"(^;)")				, TokenType::SEMICOLON},
		std::pair<std::regex, TokenType>{std::regex(R"(^\d+\b)")			, TokenType::INTEGER},
		std::pair<std::regex, TokenType>{std::regex(R"(^[a-zA-Z][\w]*\b)")	, TokenType::ID},
	};

	if (!cursor.empty()) {
		// Remove leading whitespaces
		cursor = std::regex_replace(cursor, std::regex(R"(^\s+)"), "");
		auto line = cursor;

		size_t newLinePos;
		if ((newLinePos = line.find('\n', 0)) != std::string::npos) { // NOTE This solution won't handle \n inside a string but that is not in the grammar
			line = line.substr(0, newLinePos);
		}

		for (auto& pair : tokenMap) {
			std::smatch match;
			if (std::regex_search(line, match, pair.first)) {
				nextToken = Token(pair.second, match.str());
				cursor = cursor.substr(match.str().length(), cursor.length() - match.str().length());
				return true;
			}
		}

		std::cout << "Syntax error: " << cursor << std::endl;
		return false;
	}

	nextToken = Token(TokenType::END, "");
	return true;
}

void Tokenizer::print(Token token)
{
	switch (token.type)
	{
		case TokenType::INTEGER:
			std::cout << token.value << " ";
			break;
		case TokenType::PLUS:
			std::cout << "+" << " ";
			break;
		case TokenType::MINUS:
			std::cout << "-" << " ";
			break;
		case TokenType::ID:
			std::cout << "\"" << token.value << "\"" << " ";
			break;
		case TokenType::SEMICOLON:
			std::cout << ";\n";
			break;
		case TokenType::EQUALS:
			std::cout << "=" << " ";
			break;
		case TokenType::PRINT:
			std::cout << "print:" << " ";
			break;
		case TokenType::END:
			std::cout << "\n";
			break;
	}
}