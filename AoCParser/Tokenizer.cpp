#include "Tokenizer.h"
#include <regex>
#include <vector>

void SyntaxError(std::string code)
{
	throw std::invalid_argument("Syntax error(tokenizer): invalid token { " + code + " }");
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
		std::pair<std::regex, TokenType>{std::regex(R"(^DAY\b)")							, TokenType::DAY},
		std::pair<std::regex, TokenType>{std::regex(R"(^load\b)")							, TokenType::LOAD},
		std::pair<std::regex, TokenType>{std::regex(R"(^if\b)")								, TokenType::IF},
		std::pair<std::regex, TokenType>{std::regex(R"(^else\b)")							, TokenType::IF_ELSE},
		std::pair<std::regex, TokenType>{std::regex(R"(^end\b)")							, TokenType::IF_CLOSE},
		std::pair<std::regex, TokenType>{std::regex(R"(^loop\b)")							, TokenType::LOOP},
		std::pair<std::regex, TokenType>{std::regex(R"(^times\b)")							, TokenType::LOOP_TIMES},
		std::pair<std::regex, TokenType>{std::regex(R"(^loopstop\b)")						, TokenType::LOOP_STOP},
		std::pair<std::regex, TokenType>{std::regex(R"(^LINE\b)")							, TokenType::LINE},
		std::pair<std::regex, TokenType>{std::regex(R"(^CHAR\b)")							, TokenType::CHAR},
		std::pair<std::regex, TokenType>{std::regex(R"(^lines\b)")							, TokenType::LOOP_LINES},
		std::pair<std::regex, TokenType>{std::regex(R"(^chars\b)")							, TokenType::LOOP_CHARS},
		std::pair<std::regex, TokenType>{std::regex(R"(^assert\b)")							, TokenType::ASSERT},
		std::pair<std::regex, TokenType>{std::regex(R"(^list\b)")							, TokenType::LIST},
		std::pair<std::regex, TokenType>{std::regex(R"(^sorted\b)")							, TokenType::LIST_SORTED},
		std::pair<std::regex, TokenType>{std::regex(R"(^unsorted\b)")						, TokenType::LIST_UNSORTED},
		std::pair<std::regex, TokenType>{std::regex(R"(^size\b)")							, TokenType::ARRAY_SIZE},
		std::pair<std::regex, TokenType>{std::regex(R"(^<<)")								, TokenType::LIST_ADD},
		std::pair<std::regex, TokenType>{std::regex(R"(^\()")								, TokenType::LPAREN},
		std::pair<std::regex, TokenType>{std::regex(R"(^\))")								, TokenType::RPAREN},
		std::pair<std::regex, TokenType>{std::regex(R"(^\[)")								, TokenType::LBRACKET},
		std::pair<std::regex, TokenType>{std::regex(R"(^\])")								, TokenType::RBRACKET},
		std::pair<std::regex, TokenType>{std::regex(R"(^\{)")								, TokenType::LBRACE},
		std::pair<std::regex, TokenType>{std::regex(R"(^\})")								, TokenType::RBRACE},
		std::pair<std::regex, TokenType>{std::regex(R"(^:)")								, TokenType::COLON},
		std::pair<std::regex, TokenType>{std::regex(R"(^modulo\b)")							, TokenType::MODULO},
		std::pair<std::regex, TokenType>{std::regex(R"(^as\b)")								, TokenType::CAST_AS},
		std::pair<std::regex, TokenType>{std::regex(R"(^INTEGER\b)")						, TokenType::TYPE_INTEGER},
		std::pair<std::regex, TokenType>{std::regex(R"(^STRING\b)")							, TokenType::TYPE_STRING},
		std::pair<std::regex, TokenType>{std::regex(R"(^FLOAT\b)")							, TokenType::TYPE_FLOAT},
		std::pair<std::regex, TokenType>{std::regex(R"(^is\s+DIGIT\b)")						, TokenType::IS_DIGIT},
		std::pair<std::regex, TokenType>{std::regex(R"(^is\s+ALPHA\b)")						, TokenType::IS_ALPHA},
		std::pair<std::regex, TokenType>{std::regex(R"(^==)")								, TokenType::IS_EQUAL},
		std::pair<std::regex, TokenType>{std::regex(R"(^>=)")								, TokenType::GREATER_EQUALS},
		std::pair<std::regex, TokenType>{std::regex(R"(^<=)")								, TokenType::LESS_EQUALS},
		std::pair<std::regex, TokenType>{std::regex(R"(^>)")								, TokenType::GREATER_THAN},
		std::pair<std::regex, TokenType>{std::regex(R"(^<)")								, TokenType::LESS_THAN},
		std::pair<std::regex, TokenType>{std::regex(R"(^\+)")								, TokenType::PLUS},
		std::pair<std::regex, TokenType>{std::regex(R"(^-)")								, TokenType::MINUS},
		std::pair<std::regex, TokenType>{std::regex(R"(^\*)")								, TokenType::MULTIPLY},
		std::pair<std::regex, TokenType>{std::regex(R"(^/)")								, TokenType::DIVIDE},
		std::pair<std::regex, TokenType>{std::regex(R"(^=)")								, TokenType::EQUALS},
		std::pair<std::regex, TokenType>{std::regex(R"(^;)")								, TokenType::SEMICOLON},
		std::pair<std::regex, TokenType>{std::regex(R"(^\d+\b)")							, TokenType::INTEGER},
		std::pair<std::regex, TokenType>{std::regex(R"(^[a-zA-Z][\w]*\b)")					, TokenType::ID},
	};

	{
		// Remove leading whitespaces
		cursor = std::regex_replace(cursor, std::regex(R"(^\s+)"), "");

		// Remove leading single line comments '//'
		std::smatch match;
		while (std::regex_search(cursor, match, std::regex(R"(//.*\n)"))) {
			auto start = cursor.find(match.str());
			if (start != cursor.npos) {
				cursor = cursor.replace(start, match.str().length(), "");
			}

			// Remove leading whitespaces
			cursor = std::regex_replace(cursor, std::regex(R"(^\s+)"), "");
		}
	}

	if (!cursor.empty()) {
		auto line = cursor;
				
		size_t newLinePos;
		if ((newLinePos = line.find('\n', 0)) != std::string::npos) { 
			line = line.substr(0, newLinePos);
		}
		lastLine = line;
		
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