#include "Parser.h"
#include <stdexcept> // For standard exception classes

#define REGISTER_PTR(ptr, assign) { auto* _register_ptr = ptr; nodes.push_back(_register_ptr); assign = _register_ptr; }

void SyntaxError(Token token, std::string expected)
{
	throw std::invalid_argument("Syntax error: " + token.ToString() + " " + expected);
}

bool Parser::ScanExpression(Token t, TreeNode** outNode)
{
	TreeNode* leftLogic = nullptr;
	if (ScanLogic(t, &leftLogic)) {
		*outNode = leftLogic;

		TreeNode* rightLogic = nullptr;
		while (tokenizer.PeekNextToken(t) && (
					t.type == TokenType::GREATER_THAN
				||	t.type == TokenType::GREATER_EQUALS
				||	t.type == TokenType::LESS_THAN
				||	t.type == TokenType::LESS_EQUALS
				||	t.type == TokenType::IS_EQUAL
		))
		{
			TokenType operatorType = t.type;
			tokenizer.ConsumeNext(); // Need to consume next since PeekNextToken doesn't consume.
			if (tokenizer.GetNextToken(t) && ScanLogic(t, &rightLogic)) {
				TreeNode* op = nullptr;
				if (operatorType == TokenType::GREATER_THAN) {
					op = new GREATER_THAN(leftLogic, rightLogic);
				}
				else if (operatorType == TokenType::GREATER_EQUALS) {
					op = new GREATER_EQUALS(leftLogic, rightLogic);
				}
				else if (operatorType == TokenType::LESS_THAN) {
					op = new LESS_THAN(leftLogic, rightLogic);
				}
				else if (operatorType == TokenType::LESS_EQUALS) {
					op = new LESS_EQUALS(leftLogic, rightLogic);
				}
				else if (operatorType == TokenType::IS_EQUAL) {
					op = new IS_EQUAL(leftLogic, rightLogic);
				}
				REGISTER_PTR(op, *outNode);
				leftLogic = op;
			}
			else {
				SyntaxError(t, "Expected logic operator");
				return false;
			}
		}
		return true;
	}
	return false;
}

bool Parser::ScanLogic(Token t, TreeNode** outNode)
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
		while (tokenizer.PeekNextToken(t) && (t.type == TokenType::MULTIPLY || t.type == TokenType::DIVIDE || t.type == TokenType::MODULO))
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
				else if (operatorType == TokenType::MODULO) {
					op = new MODULO(leftFactor, rightFactor);
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
	else if (t.type == TokenType::LPAREN)
	{
		if (tokenizer.GetNextToken(t) && !ScanExpression(t, outNode))
		{
			SyntaxError(t, "Expected expression");
			return false;
		}

		if (tokenizer.GetNextToken(t) && t.type != TokenType::RPAREN)
		{
			SyntaxError(t, "Expected ')'");
			return false;
		}

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
			REGISTER_PTR(new PRINT_ID(id), *outNode);
			return true;
		}
		else if (ScanString(t, &id)) {
			REGISTER_PTR(new PRINT_STR(id), *outNode);
			return true;
		}
		else {
			SyntaxError(t, "Expected identifier or string");
		}
	}
	return false;
}

bool Parser::ScanString(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::STRING) {
		std::string str = t.value.substr(1, t.value.length() - 2);
		REGISTER_PTR(new STRING(str), *outNode);
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
			SyntaxError(t, "Expected expression");
		}
	}
	return false;
}


bool Parser::ScanLoop(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::LOOP) {
		TreeNode* times;
		if (tokenizer.GetNextToken(t) && ScanExpression(t, &times)) {
			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::LOOP_TIMES)) {
				SyntaxError(t, "Expected 'times'");
				return false;
			}

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

			if (t.type != TokenType::LOOP_STOP) {
				SyntaxError(t, "Expected 'loopstop'");
				return false;
			}

			REGISTER_PTR(new LOOP(times, statements), *outNode);
			return true;
		}
		else {
			SyntaxError(t, "Expected expression");
		}
	}
	return false;
}

bool Parser::ScanAssert(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::ASSERT) {
		TreeNode* condition;
		if (tokenizer.GetNextToken(t) && ScanExpression(t, &condition)) {
			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::COLON)) {
				SyntaxError(t, "Expected colon ':'");
				return false;
			}

			TreeNode* str;
			if (tokenizer.GetNextToken(t) && ScanString(t, &str)) {
				REGISTER_PTR(new ASSERT(condition, str), *outNode);
				return true;
			}
			else {
				SyntaxError(t, "Expected string");
			}
		}
		else {
			SyntaxError(t, "Expected expression");
		}
	}
	return false;
}

bool Parser::ScanStatement(Token t, TreeNode** outNode, bool programStatement)
{
	TreeNode* statement = nullptr;
	if (ScanAssignment(t, &statement) 
		|| ScanPrint(t, &statement) 
		|| ScanLoad(t, &statement) 
		|| ScanIf(t, &statement) 
		|| ScanLoop(t, &statement)
		|| ScanAssert(t, &statement)
		) {
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



Parser::Parser(std::string code, bool printSyntax) : tokenizer(code), ast(nullptr)
{
	Token t;
	TreeNode* statement;
	try {
		while (tokenizer.GetNextToken(t) && ScanStatement(t, &statement))
		{
			statements.push_back(statement);
			if (printSyntax) { std::cout << "\t\t"; statement->print(); }
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
