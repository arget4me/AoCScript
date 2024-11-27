#include "Tokenizer.h"
#include <regex>
#include <vector>

void SyntaxError(std::string code)
{
	throw std::invalid_argument("Syntax error: invalid token { " + code + " }");
}

bool Tokenizer::checkTokenMap(const std::vector<std::pair<std::regex, TokenType>>& tokenMap, std::string& line)
{
	for (auto& pair : tokenMap) {
		std::smatch match;
		if (std::regex_search(line, match, pair.first)) {
			nextToken = Token(pair.second, match.str());
			cursor = cursor.substr(match.str().length(), cursor.length() - match.str().length());
			return true;
		}
	}
	return false;
}

bool Tokenizer::scanToken()
{
	static const std::vector<std::pair<std::regex, TokenType>> multilineTokenMap = {
	};

	static const std::vector<std::pair<std::regex, TokenType>> singlelineTokenMap = {
		std::pair<std::regex, TokenType>{std::regex(R"(^".*")")								, TokenType::STRING},
		std::pair<std::regex, TokenType>{std::regex(R"(^simon says\b|^print\b)")			, TokenType::PRINT},
		std::pair<std::regex, TokenType>{std::regex(R"(^load\b)")							, TokenType::LOAD},
		std::pair<std::regex, TokenType>{std::regex(R"(^if\b)")								, TokenType::IF},
		std::pair<std::regex, TokenType>{std::regex(R"(^else\b)")							, TokenType::IF_ELSE},
		std::pair<std::regex, TokenType>{std::regex(R"(^end\b)")							, TokenType::IF_CLOSE},
		std::pair<std::regex, TokenType>{std::regex(R"(^:)")								, TokenType::COLON},
		std::pair<std::regex, TokenType>{std::regex(R"(^\+)")								, TokenType::PLUS},
		std::pair<std::regex, TokenType>{std::regex(R"(^-)")								, TokenType::MINUS},
		std::pair<std::regex, TokenType>{std::regex(R"(^\*)")								, TokenType::MULTIPLY},
		std::pair<std::regex, TokenType>{std::regex(R"(^/)")								, TokenType::DIVIDE},
		std::pair<std::regex, TokenType>{std::regex(R"(^=)")								, TokenType::EQUALS},
		std::pair<std::regex, TokenType>{std::regex(R"(^;)")								, TokenType::SEMICOLON},
		std::pair<std::regex, TokenType>{std::regex(R"(^\d+\b)")							, TokenType::INTEGER},
		std::pair<std::regex, TokenType>{std::regex(R"(^[a-zA-Z][\w]*\b)")					, TokenType::ID},
	};

	if (!cursor.empty()) {
		// Remove leading whitespaces
		cursor = std::regex_replace(cursor, std::regex(R"(^\s+)"), "");
		auto line = cursor;
				
		size_t newLinePos;
		if ((newLinePos = line.find('\n', 0)) != std::string::npos) { 
			line = line.substr(0, newLinePos);
		}
		
		if (checkTokenMap(singlelineTokenMap, line)) {
			return true;
		}

		if (checkTokenMap(multilineTokenMap, cursor)) { // Must check these cases before the newline token is skipped
			return true;
		}

		SyntaxError(cursor);
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
			std::cout << token.value << " ";
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
		case TokenType::LOAD:
			std::cout << "load:" << " ";
			break;
		case TokenType::STRING:
			std::cout << "\'" << token.value.substr(1, token.value.length() - 2) << "\'" << " ";

			break;
		case TokenType::END:
			std::cout << "\n";
			break;
		default:
			std::cout << "UNKNOWN:{" << token.value << "} ";

	}
}