#include "Parser.h"
#include <stdexcept> // For standard exception classes

// For Reading entire file to string
#include <fstream>
#include <sstream>

#define REGISTER_PTR(ptr, assign) { auto* _register_ptr = ptr; nodes.push_back(_register_ptr); assign = _register_ptr; }

bool ReadFile(const std::string& filePath, std::string& fileContents) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		throw std::invalid_argument("Load input file not found: " + filePath);
		return false;
	}

	// Read the entire file into a string
	std::ostringstream buffer;
	buffer << file.rdbuf();

	fileContents = buffer.str();

	file.close();
	return true;
}

void SyntaxError(Tokenizer& t, Token token, std::string expected)
{
	throw std::invalid_argument("Syntax error: " + token.ToString() + " " + expected + ".\n At: " + t.GetLastLine());
}

void RuntimeError(std::string error_message)
{
	throw std::invalid_argument("Runtime error: {" + error_message + "}");
}

bool Parser::ScanExpression(Token t, TreeNode** outNode)
{
	TreeNode* leftLogic = nullptr;
	if (ScanLogic(t, &leftLogic)) {
		*outNode = leftLogic;

		if (tokenizer.PeekNextToken(t) && (
			t.type == TokenType::IS_DIGIT
			|| t.type == TokenType::IS_ALPHA
			))
		{
			tokenizer.GetNextToken(t);
			TreeNode* op = nullptr;
			if (t.type == TokenType::IS_DIGIT) {
				op = new IS_DIGIT(leftLogic);
			}
			else if (t.type == TokenType::IS_ALPHA) {
				op = new IS_ALPHA(leftLogic);
			}
			REGISTER_PTR(op, *outNode);
		}
		else {
			TreeNode* rightLogic = nullptr;
			while (tokenizer.PeekNextToken(t) && (
				t.type == TokenType::GREATER_THAN
				|| t.type == TokenType::GREATER_EQUALS
				|| t.type == TokenType::LESS_THAN
				|| t.type == TokenType::LESS_EQUALS
				|| t.type == TokenType::IS_EQUAL
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
					SyntaxError(tokenizer, t, "Expected logic operator");
					return false;
				}
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
				SyntaxError(tokenizer, t, "Expected term");
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

		if (tokenizer.PeekNextToken(t) && t.type == TokenType::CAST_AS)
		{
			tokenizer.ConsumeNext();
			if (tokenizer.GetNextToken(t))
			{
				if (t.type == TokenType::TYPE_INTEGER) {
					REGISTER_PTR(new CAST(leftFactor, VariableType::INTEGER), *outNode);
				}
				else if (t.type == TokenType::TYPE_STRING) {
					REGISTER_PTR(new CAST(leftFactor, VariableType::STRING), *outNode);
				}
				else if (t.type == TokenType::TYPE_FLOAT) {
					REGISTER_PTR(new CAST(leftFactor, VariableType::FLOAT), *outNode);
				}
				else {
					SyntaxError(tokenizer, t, "Expected Cast TYPE");
				}
				return true;
			}
			SyntaxError(tokenizer, t, "Expected Cast TYPE but were no tokens left!");
		}
		else {
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
					SyntaxError(tokenizer, t, "Expected term");
					return false;
				}
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
		TreeNode* id = *outNode;
		// TODO PEEK NEXT TOKEN FOR "[" EXPRESSION "]"
		if (tokenizer.PeekNextToken(t) && t.type == TokenType::LBRACKET)
		{
			tokenizer.ConsumeNext();
			TreeNode* expression = nullptr;
			if (tokenizer.GetNextToken(t) && !ScanExpression(t, &expression))
			{
				SyntaxError(tokenizer, t, "Expected expression");
				return false;
			}

			if (tokenizer.GetNextToken(t) && t.type != TokenType::RBRACKET)
			{
				SyntaxError(tokenizer, t, "Expected ')'");
				return false;
			}

			REGISTER_PTR(new ARRAY_INDEXING(id, expression), *outNode);
			return true;
		}
		else if (tokenizer.PeekNextToken(t) && t.type == TokenType::ARRAY_SIZE)
		{
			tokenizer.ConsumeNext();
			REGISTER_PTR(new ARRAY_SIZE(id), *outNode);
			return true;
		}
		return true;
	}
	else if (t.type == TokenType::LPAREN)
	{
		if (tokenizer.GetNextToken(t) && !ScanExpression(t, outNode))
		{
			SyntaxError(tokenizer, t, "Expected expression");
			return false;
		}

		if (tokenizer.GetNextToken(t) && t.type != TokenType::RPAREN)
		{
			SyntaxError(tokenizer, t, "Expected ')'");
			return false;
		}

		return true;
	}
	else if (ScanNegate(t, outNode)) {
		return true;
	}
	else if (ScanString(t, outNode)) {
		return true;
	}
	return false;
}

bool Parser::ScanNegate(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::MINUS)
	{
		TreeNode* negateFactor = nullptr;
		if (tokenizer.GetNextToken(t) && ScanFactor(t, &negateFactor))
		{
			REGISTER_PTR(new NEGATE(negateFactor), *outNode);
			return true;
		}
		SyntaxError(tokenizer, t, "Expected factor");
	}
	return false;
}

bool Parser::ScanAssignment(Token t, TreeNode** outNode)
{
	TreeNode* id = nullptr;
	if(ScanID(t, &id)) {
		if (!tokenizer.GetNextToken(t)) {
			SyntaxError(tokenizer, t, "Expected more tokens after ID assignment");
		}

		if (t.type == TokenType::EQUALS) {
			TreeNode* expression = nullptr;
			if (tokenizer.GetNextToken(t) && (ScanExpression(t, &expression) || ScanString(t, &expression))) {
				REGISTER_PTR(new EQUALS(id, expression), *outNode);
				return true;
			}
			else {
				SyntaxError(tokenizer, t, "Expected expression for assignment");
			}
		}

		// Indexed Assignment
		if (t.type == TokenType::LBRACKET) {
			std::string id_name = static_cast<ID*>(id)->str;
			if (declaredLists.find(id_name) == declaredLists.end())
			{
				SyntaxError(tokenizer, t, "Using undeclared list : " + id_name);
			}

			TreeNode* index = nullptr;
			if (!(tokenizer.GetNextToken(t) && ScanExpression(t, &index))) {
				SyntaxError(tokenizer, t, "Expected index expression for assignment");
				return false;
			}

			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::RBRACKET)) {
				SyntaxError(tokenizer, t, "Expected closing ']'");
				return false;
			}

			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::EQUALS)) {
				SyntaxError(tokenizer, t, "Expected '=' operator for assignment");
				return false;
			}

			TreeNode* expression = nullptr;
			if (tokenizer.GetNextToken(t) && ScanExpression(t, &expression)) {
				REGISTER_PTR(new EQUALS_INDEXED(id, index, expression), *outNode);
				return true;
			}
			else {
				SyntaxError(tokenizer, t, "Expected expression for list assignment");
			}
		}


		// ListAssignment
		if (t.type == TokenType::LIST_ADD) {
			std::string id_name = static_cast<ID*>(id)->str;
			if (declaredLists.find(id_name) == declaredLists.end())
			{
				SyntaxError(tokenizer, t, "Using undeclared list : " + id_name);
			}

			TreeNode* expression = nullptr;
			if (tokenizer.GetNextToken(t) && ScanExpression(t, &expression)) {
				REGISTER_PTR(new LIST_ADD(id, expression), *outNode);
				return true;
			}
			else {
				SyntaxError(tokenizer, t, "Expected expression for list assignment");
			}
		}

		SyntaxError(tokenizer, t, "Invalid assignment syntax");
	}
	return false;
}

bool Parser::ScanListDeclaration(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::LIST_SORTED || t.type == TokenType::LIST_UNSORTED)
	{
		bool isSorted = t.type == TokenType::LIST_SORTED;
		if (tokenizer.GetNextToken(t))
		{
			VariableType varType = VariableType::INTEGER;
			if (t.type == TokenType::TYPE_INTEGER) {
				varType = VariableType::INTEGER;
			}
			else if (t.type == TokenType::TYPE_STRING) {
				varType = VariableType::STRING;
			}
			else if (t.type == TokenType::TYPE_FLOAT) {
				varType = VariableType::FLOAT;
			}

			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::LIST))
			{
				SyntaxError(tokenizer, t, "Expected 'list' keyword");
			}

			TreeNode* id = nullptr;
			if (tokenizer.GetNextToken(t) && ScanID(t, &id)) {
				std::string id_name = static_cast<ID*>(id)->str;
				if (declaredLists.find(id_name) != declaredLists.end())
				{
					SyntaxError(tokenizer, t, "Duplicate List declarations! : " + id_name);
				}

				declaredLists[id_name] = 0;
				REGISTER_PTR(new LIST_CREATE(id, isSorted, varType), *outNode);
				return true;
			}
			SyntaxError(tokenizer, t, "Expected variable name for list declaration");
		}
		SyntaxError(tokenizer, t, "VariableType is required for declating a list");
	}
	return false;
}

bool Parser::ScanID(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::ID) {
		REGISTER_PTR(new ID(t.value), *outNode);
		return true;
	}
	else if(t.type == TokenType::LINE) {
		REGISTER_PTR(new ID("LINE"), *outNode);
		return true;
	}
	else if(t.type == TokenType::CHAR) {
		REGISTER_PTR(new ID("CHAR"), *outNode);
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
		else if (t.type == TokenType::DAY) {
			REGISTER_PTR(new PRINT_DAY(), *outNode);
			return true;
		} 
		else {
			SyntaxError(tokenizer, t, "Expected identifier or string or DAY");
		}
	}
	return false;
}

bool Parser::ScanBreak(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::LOOP_BREAK) {
		REGISTER_PTR(new BREAK(), *outNode);
		return true;
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
			SyntaxError(tokenizer, t, "Expected string");
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
				SyntaxError(tokenizer, t, "Expected colon ':'");
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
				SyntaxError(tokenizer, t, "Expected 'else'");
				return false;
			}

			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::COLON)) {
				SyntaxError(tokenizer, t, "Expected colon ':'");
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
				SyntaxError(tokenizer, t, "Expected 'end'");
				return false;
			}

			REGISTER_PTR(new IF(condition, statements, else_statements), *outNode);
			return true;
		}
		else {
			SyntaxError(tokenizer, t, "Expected expression");
		}
	}
	return false;
}


bool Parser::ScanLoop(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::LOOP) {
		if (!tokenizer.GetNextToken(t)) {
			SyntaxError(tokenizer, t, "loop requires an iterator or an expression");
		}

		TreeNode* id = nullptr;
		Token nextToken;
		if (t.type != TokenType::CHAR 
			&& (ScanID(t, &id) && tokenizer.PeekNextToken(nextToken) && nextToken.type == TokenType::LOOP_CHARS)) {
			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::LOOP_CHARS)) {
				SyntaxError(tokenizer, t, "Expected 'chars'");
				return false;
			}

			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::COLON)) {
				SyntaxError(tokenizer, t, "Expected colon ':'");
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
				SyntaxError(tokenizer, t, "Expected 'loopstop'");
				return false;
			}

			REGISTER_PTR(new LOOP_ITERATOR(id, statements), *outNode);
			return true;
		}

		if (id != nullptr) {
			// Only case id is allocated is if ScanID succeded but the other conditions weren't met. Thus 'id' is not accurately parsed.
			// Parser destructor will take care of it.
			id = nullptr;
		}

		TreeNode* times = id;
		if (ScanExpression(t, &times)) {
			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::LOOP_TIMES)) {
				SyntaxError(tokenizer, t, "Expected 'times'");
				return false;
			}

			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::COLON)) {
				SyntaxError(tokenizer, t, "Expected colon ':'");
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
				SyntaxError(tokenizer, t, "Expected 'loopstop'");
				return false;
			}

			REGISTER_PTR(new LOOP(times, statements), *outNode);
			return true;
		}
		
		if (t.type == TokenType::DAY) {
			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::LOOP_LINES)) {
				SyntaxError(tokenizer, t, "Expected 'lines'");
				return false;
			}

			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::COLON)) {
				SyntaxError(tokenizer, t, "Expected colon ':'");
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
				SyntaxError(tokenizer, t, "Expected 'loopstop'");
				return false;
			}

			REGISTER_PTR(new LOOP_DAY(statements), *outNode);
			return true;
		}

		SyntaxError(tokenizer, t, "Expected expression or iterator for loop");
	}
	return false;
}

bool Parser::ScanAssert(Token t, TreeNode** outNode)
{
	if (t.type == TokenType::ASSERT) {
		TreeNode* condition;
		if (tokenizer.GetNextToken(t) && ScanExpression(t, &condition)) {
			if (!(tokenizer.GetNextToken(t) && t.type == TokenType::COLON)) {
				SyntaxError(tokenizer, t, "Expected colon ':'");
				return false;
			}

			TreeNode* str;
			if (tokenizer.GetNextToken(t) && ScanString(t, &str)) {
				REGISTER_PTR(new ASSERT(condition, str), *outNode);
				return true;
			}
			else {
				SyntaxError(tokenizer, t, "Expected string");
			}
		}
		else {
			SyntaxError(tokenizer, t, "Expected expression");
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
		|| ScanListDeclaration(t, &statement)
		|| ScanBreak(t, &statement)
		) {
		if (tokenizer.GetNextToken(t) && t.type == TokenType::SEMICOLON) {
			REGISTER_PTR(new Statement(statement), *outNode);
			return true;
		}
		else {
			SyntaxError(tokenizer, t, "Expected ; (semicolon)");
		}
		
	}
	return false;
}

Parser::Parser(std::string code, bool printSyntax) : tokenizer(code), ast(nullptr)
{
	Token t;
	TreeNode* statement;
	bool success = true;
	try {
		while (tokenizer.GetNextToken(t) && ScanStatement(t, &statement))
		{
			statements.push_back(statement);
			PushConsoleColor(CONSOLE_COLOR::YELLOW);
			if (printSyntax) { std::cout << "\t\t"; statement->print(); }
			PopConsoleColor();
			statement->eval(&globals);
		}
		if (t.type != TokenType::END) {
			SyntaxError(tokenizer, t, "Expected no more statements but received more.");
		}

	}
	catch (const std::invalid_argument& e) {
		success = false;
		PushConsoleColor(CONSOLE_COLOR::RED);
		std::cerr << e.what() << std::endl;
		PopConsoleColor();
		throw std::invalid_argument("d");
	}
}

Parser::~Parser()
{
	for (TreeNode* node : nodes)
	{
		delete node;
	}
}

void LOAD::eval(RuntimeGlobals* globals)
{
	str->eval(globals);
	globals->DayFileName = globals->pop_var().strValue;
	if (!ReadFile(globals->DayFileName, globals->DayString))
	{
		RuntimeError("Could not load Day input from file {" + globals->DayFileName + "}");
	}
	
	auto splitByLines = [](const std::string& input, std::vector<std::string>& outLines) {
		outLines.clear();
		std::istringstream stream(input);
		std::string line;

		while (std::getline(stream, line)) {
			outLines.push_back(line); // Add each line to the vector
		}
	};
	splitByLines(globals->DayString, globals->DayLines);
}

void CAST::eval(RuntimeGlobals* globals) {
	left->eval(globals);
	StackVariable var = globals->pop_var();
	StackVariable result = var;

	VariableType fromType = var.type;
	VariableType toType = type;

	switch (fromType)
	{
	case VariableType::INTEGER:
	{
		switch (toType)
		{
		case VariableType::INTEGER:
			globals->push_var(result);
			break;
		case VariableType::STRING:
			globals->push_var(std::to_string(var.intValue));
			break;
		case VariableType::FLOAT:
			globals->push_var(static_cast<float>(var.intValue));
			break;
		default:
			break;
		}
	}break;
	case VariableType::STRING:
	{
		switch (toType)
		{
		case VariableType::INTEGER:
			globals->push_var(std::stoi(var.strValue));
			break;
		case VariableType::STRING:
			globals->push_var(var);
			break;
		case VariableType::FLOAT:
			globals->push_var(std::stof(var.strValue));
			break;
		default:
			break;
		}
	}break;
	case VariableType::FLOAT:
	{
		switch (toType)
		{
		case VariableType::INTEGER:
			globals->push_var(static_cast<int>(var.fltValue));
			break;
		case VariableType::STRING:
			globals->push_var(std::to_string(var.fltValue));
			break;
		case VariableType::FLOAT:
			globals->push_var(var);
			break;
		default:
			break;
		}
	}break;
	}
}

void List::push_var(StackVariable var)
{
	if (var.type != type) {
		RuntimeError("Can't add value of type {" + VariableTypeToString(var.type) + "} to list" + "<" + VariableTypeToString(type) + ">");
	}
	list.push_back(var);
}

StackVariable List::pop_var()
{
	if (list.size() == 0) return StackVariable(0);
	StackVariable result = list.back();
	list.pop_back();
	return result;
}

void List::set_var(int index, StackVariable expressionVar)
{
	if (expressionVar.type != type) {
		RuntimeError("Can't add value of type {" + VariableTypeToString(expressionVar.type) + "} to list" + "<" + VariableTypeToString(type) + ">");
	}

	if (index >= 0 && index < list.size()) {
		list[index] = expressionVar;
	}
}

void SortedList::push_var(StackVariable var)
{
	if (var.type != type) {
		RuntimeError("Can't add value of type {" + VariableTypeToString(var.type) + "} to list" + "<" + VariableTypeToString(type) + ">");
	}

	auto insertion_sort = [](std::vector<StackVariable>& vec, StackVariable value) {
		auto it = std::lower_bound(vec.begin(), vec.end(), value);
		vec.insert(it, value);
	};

	insertion_sort(list, var);
}

void SortedList::set_var(int index, StackVariable expressionVar)
{
	if (expressionVar.type != type) {
		RuntimeError("Can't add value of type {" + VariableTypeToString(expressionVar.type) + "} to list" + "<" + VariableTypeToString(type) + ">");
	}

	if (index >= 0 && index < list.size()) {
		list[index] = expressionVar;
	}

	std::sort(list.begin(), list.end());
}

void RuntimeGlobals::push_var(StackVariable var)
{
	stack.push_back(var);
}

StackVariable RuntimeGlobals::pop_var()
{
	if (stack.size() == 0) return StackVariable(0);
	StackVariable result = stack.back();
	stack.pop_back();
	return result;
}
