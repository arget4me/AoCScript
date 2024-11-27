#include "Parser.h"
#include <stdexcept> // For standard exception classes

#define REGISTER_PTR(ptr, assign) { auto* _register_ptr = ptr; nodes.push_back(_register_ptr); assign = _register_ptr; }

void SyntaxError(Token token, std::string expected)
{
	throw std::invalid_argument("Syntax error: " + token.ToString() + " " + expected);
}

bool Parser::ScanExpression(Token t, TreeNode** outNode)
{
	TreeNode* leftTerm = nullptr;
	if (ScanTerm(t, &leftTerm)) {
		*outNode = leftTerm;

		TreeNode* rightTerm = nullptr;
		while (tokenizer.PeekNextToken(t) && (t.type == TokenType::PLUS || t.type == TokenType::MINUS))
		{
			TokenType operatorType = t.type;
			tokenizer.ConsumeNext(); // Need to consume next since PeekNextToken doesn't consume.
			if (tokenizer.GetNextToken(t) && ScanTerm(t, &rightTerm)) {
				TreeNode* op = nullptr;
				if (operatorType == TokenType::PLUS) {
					op = new ADD(leftTerm, rightTerm);
				}
				else if (operatorType == TokenType::MINUS) {
					op = new SUBTRACT(leftTerm, rightTerm);
				}
				REGISTER_PTR(op, *outNode);
				leftTerm = op;
			}
			else {
				SyntaxError(t, "Expected term");
				return false;
			}
		}
		return true;
	}
	return false;
}

bool Parser::ScanTerm(Token t, TreeNode** outNode)
{
	TreeNode* leftFactor = nullptr;
	if (ScanFactor(t, &leftFactor)) {
		*outNode = leftFactor;

		TreeNode* rightFactor = nullptr;
		while (tokenizer.PeekNextToken(t) && (t.type == TokenType::MULTIPLY || t.type == TokenType::DIVIDE))
		{
			TokenType operatorType = t.type;
			tokenizer.ConsumeNext(); // Need to consume next since PeekNextToken doesn't consume.
			if (tokenizer.GetNextToken(t) && ScanFactor(t, &rightFactor)) {
				OPERATOR* op = nullptr;
				if (operatorType == TokenType::MULTIPLY) {
					op = new MULT(leftFactor, rightFactor);
				}
				else if (operatorType == TokenType::DIVIDE) {
					op = new DIV(leftFactor, rightFactor);
				}
				REGISTER_PTR(op, *outNode);
				leftFactor = op;
			}
			else {
				SyntaxError(t, "Expected term");
				return false;
			}
		}
		return true;
	}
	return false;
}

bool Parser::ScanFactor(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::INTEGER) {
		REGISTER_PTR(new INTEGER(std::stoi(t.value)), *outNode);
		return true;
	}
	else if (ScanID(t, outNode)) {
		return true;
	}
	//else if ( ScanExpression(t, )) //    "("   Expression    ")"
	return false;
}

bool Parser::ScanAssignment(Token t, TreeNode** outNode)
{
	TreeNode* id = nullptr;
	if(ScanID(t, &id)) {
		if (tokenizer.GetNextToken(t) && t.type == TokenType::EQUALS) {
			TreeNode* expression = nullptr;
			if (tokenizer.GetNextToken(t) && ScanExpression(t, &expression)) {
				REGISTER_PTR(new EQUALS(id, expression), *outNode);
				return true;
			}
			else {
				SyntaxError(t, "Expected expression");
			}
		}
	}
	return false;
}

bool Parser::ScanID(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::ID) {
		REGISTER_PTR(new ID(t.value), *outNode);
		return true;
	}
	return false;
}

bool Parser::ScanPrint(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::PRINT) {
		TreeNode* id;
		if (tokenizer.GetNextToken(t) && ScanID(t, &id)) {
			REGISTER_PTR(new PRINT(id), *outNode);
			return true;
		}
		else {
			SyntaxError(t, "Expected identifier");
		}
	}
	return false;
}

bool Parser::ScanString(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::STRING) {
		REGISTER_PTR(new STRING(t.value), *outNode);
		return true;
	}
	return false;
}

bool Parser::ScanLoad(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::LOAD) {
		TreeNode* str;
		if (tokenizer.GetNextToken(t) && ScanString(t, &str)) {
			REGISTER_PTR(new LOAD(str), *outNode);
			return true;
		}
		else {
			SyntaxError(t, "Expected string");
		}
	}
	return false;
}

bool Parser::ScanIf(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::IF) {
		TreeNode* condition;
		if (tokenizer.GetNextToken(t) && ScanExpression(t, &condition)) {
			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::COLON)) {
				SyntaxError(t, "Expected colon ':'");
				return false;
			}
			
			std::vector<TreeNode*> statements;
			{
				TreeNode* statement = nullptr;
				while (tokenizer.GetNextToken(t) && ScanStatement(t, &statement, false)) {
					statements.push_back(statement);
				} // Will end on GetNextToken being called and ScanExpression failing, don't have to call get next token again.
			}

			if (t.type != TokenType::IF_ELSE) {
				SyntaxError(t, "Expected 'else'");
				return false;
			}

			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::COLON)) {
				SyntaxError(t, "Expected colon ':'");
				return false;
			}

			std::vector<TreeNode*> else_statements;
			{
				TreeNode* else_statement = nullptr;
				while (tokenizer.GetNextToken(t) && ScanStatement(t, &else_statement, false)) {
					else_statements.push_back(else_statement);
				} // Will end on GetNextToken being called and ScanExpression failing, don't have to call get next token again.
			}

			if (t.type != TokenType::IF_CLOSE) {
				SyntaxError(t, "Expected 'end'");
				return false;
			}

			REGISTER_PTR(new IF(condition, statements, else_statements), *outNode);
			return true;
		}
		else {
			SyntaxError(t, "Expected string");
		}
	}
	return false;
}

bool Parser::ScanStatement(Token t, TreeNode** outNode, bool programStatement)
{
	TreeNode* statement = nullptr;
	if (ScanAssignment(t, &statement) || ScanPrint(t, &statement) || ScanLoad(t, &statement) || ScanIf(t, &statement)) {
		if (tokenizer.GetNextToken(t) && t.type == TokenType::SEMICOLON) {
			if (programStatement) {
				*outNode = new Statement(statement);
			}
			else {
				REGISTER_PTR(new Statement(statement), *outNode);
			}
			return true;
		}
		else {
			SyntaxError(t, "Expected ; (semicolon)");
		}
		
	}
	return false;
}



Parser::Parser(std::string code) : tokenizer(code), ast(nullptr)
{
	Token t;
	TreeNode* statement;
	try {
		while (tokenizer.GetNextToken(t) && ScanStatement(t, &statement))
		{
			statements.push_back(statement);
			//statement->print();
			statement->eval();
		}
	}
	catch (const std::invalid_argument& e) {
		std::cerr << e.what() << std::endl;
	}
}

Parser::~Parser()
{
	for (TreeNode* node : nodes)
	{
		delete node;
	}
}
