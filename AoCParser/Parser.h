#pragma once
#include "Tokenizer.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <iomanip> // For manipulators : std::setprecision(2)
#include "PrintHelper.h"

bool ReadFile(const std::string& filePath, std::string& fileContents);
void SyntaxError(Token token, std::string expected);
void RuntimeError(std::string expected);

enum class VariableType
{
	INTEGER,
	STRING,
	FLOAT // Being added in DLC?
};

static std::string VariableTypeToString(VariableType type)
{
	switch (type)
	{
	case VariableType::INTEGER:
		return "INTEGER";
		break;
	case VariableType::STRING:
		return "STRING";
		break;
	case VariableType::FLOAT:
		return "FLOAT";
		break;
	}

	return "UNKNOWN-UNIMPLEMENTED!!";
}

struct StackVariable {
	StackVariable() : StackVariable(0) { }
	StackVariable(int intValue)
		: type(VariableType::INTEGER), intValue(intValue), strValue(""), fltValue(0.0f) {}

	StackVariable(std::string strValue)
		: type(VariableType::STRING), intValue(0), strValue(strValue), fltValue(0.0f) {}

	StackVariable(float fltValue)
		: type(VariableType::FLOAT), intValue(0), strValue(""), fltValue(fltValue) {}

	VariableType type;

	int intValue;
	std::string strValue;
	float fltValue;
};

class RuntimeGlobals
{
public:
	RuntimeGlobals() {
		stack = {};
		variables = {};
		DayLines = {};
		DayString = "";
		DayFileName = "";
	}

	~RuntimeGlobals() {
	}	

	std::vector<StackVariable> stack;
	std::map<std::string, StackVariable> variables;

	std::vector<std::string> DayLines;
	std::string DayString;
	std::string DayFileName;

	void push_var(int var) { push_var(StackVariable(var)); };
	void push_var(std::string var) { push_var(StackVariable(var)); };
	void push_var(float var) { push_var(StackVariable(var)); };
	void push_var(StackVariable var);
	StackVariable pop_var();
};

class TreeNode
{
public:
	virtual void print() = 0;
	virtual void eval(RuntimeGlobals* globals) = 0;
};

class Statement : public TreeNode
{
public:
	Statement(TreeNode* statement) : statement(statement) {}
	TreeNode* statement;
	virtual void print() override {
		statement->print(); std::cout << ";\n";
	}

	virtual void eval(RuntimeGlobals* globals) override {
		statement->eval(globals);
		globals->pop_var();
	}
};

class OPERATOR : public TreeNode
{
public:
	OPERATOR(TreeNode* left, TreeNode* right) : left(left), right(right) {}
	TreeNode* left;
	TreeNode* right;
};


class ADD : public OPERATOR
{
public:
	ADD(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " + "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		int left = globals->pop_var().intValue;

		right->eval(globals);
		int right = globals->pop_var().intValue;
		
		int result = left + right;
		globals->push_var(result);
	}
};

class SUBTRACT : public OPERATOR
{
public:
	SUBTRACT(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " - "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		int left = globals->pop_var().intValue;

		right->eval(globals);
		int right = globals->pop_var().intValue;

		int result = left - right;
		globals->push_var(result);
	}
};

class MULT : public OPERATOR
{
public:
	MULT(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " * "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		int left = globals->pop_var().intValue;

		right->eval(globals);
		int right = globals->pop_var().intValue;

		int result = left * right;
		globals->push_var(result);
	}
};

class DIV : public OPERATOR
{
public:
	DIV(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " / "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		int left = globals->pop_var().intValue;

		right->eval(globals);
		int right = globals->pop_var().intValue;

		int result = left / right;
		globals->push_var(result);
	}
};

class MODULO : public OPERATOR
{
public:
	MODULO(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " MODULO "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		int left = globals->pop_var().intValue;

		right->eval(globals);
		int right = globals->pop_var().intValue;

		int result = left % right;
		globals->push_var(result);
	}
};

class CAST : public TreeNode
{
public:
	CAST(TreeNode* left, VariableType type) : left(left), type(type) {}
	TreeNode* left;
	VariableType type;

	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " CAST TO "; 
		std::cout << VariableTypeToString(type);
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override;
};

class GREATER_THAN : public OPERATOR
{
public:
	GREATER_THAN(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " > "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		int left = globals->pop_var().intValue;

		right->eval(globals);
		int right = globals->pop_var().intValue;

		int result = left > right;
		globals->push_var(result);
	}
};

class GREATER_EQUALS : public OPERATOR
{
public:
	GREATER_EQUALS(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " >= "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		int left = globals->pop_var().intValue;

		right->eval(globals);
		int right = globals->pop_var().intValue;

		int result = left >= right;
		globals->push_var(result);
	}
};

class LESS_THAN : public OPERATOR
{
public:
	LESS_THAN(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " < "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		int left = globals->pop_var().intValue;

		right->eval(globals);
		int right = globals->pop_var().intValue;

		int result = left < right;
		globals->push_var(result);
	}
};

class LESS_EQUALS : public OPERATOR
{
public:
	LESS_EQUALS(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " <= "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		int left = globals->pop_var().intValue;

		right->eval(globals);
		int right = globals->pop_var().intValue;

		int result = left <= right;
		globals->push_var(result);
	}
};

class IS_EQUAL : public OPERATOR
{
public:
	IS_EQUAL(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " == "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		int left = globals->pop_var().intValue;

		right->eval(globals);
		int right = globals->pop_var().intValue;

		int result = left == right;
		globals->push_var(result);
	}
};

class NEGATE : public TreeNode
{
public:
	NEGATE(TreeNode* arg) :arg(arg) {}
	TreeNode* arg;

	virtual void print() override {
		std::cout << "(";
		std::cout << "-"; arg->print();
		std::cout << ")";
	}

	virtual void eval(RuntimeGlobals* globals) override { 
		arg->eval(globals);
		int arg_value = globals->pop_var().intValue;
		globals->push_var(-arg_value);
	}
};

class ID : public TreeNode
{
public:
	ID(std::string str) : str(str) {}
	std::string str;

public:
	virtual void print() override { std::cout << str; }
	virtual void eval(RuntimeGlobals* globals) override 
	{
		auto found = globals->variables.find(str);
		if (found != globals->variables.end()) {
			VariableType type = globals->variables[str].type;
			if (type == VariableType::INTEGER) {
				globals->push_var(globals->variables[str].intValue);
			}
			else if (type == VariableType::STRING) {
				globals->push_var(globals->variables[str].strValue);
			}
			else if (type == VariableType::FLOAT) {
				globals->push_var(globals->variables[str].fltValue);
			}
		}
		else {
			// TODO Throw runtime error
		}
	}
};

static std::string ColorizeString(std::string str)
{
	auto replaceAll = [](std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Move past the replacement to avoid infinite loop
		}
	};

	auto replaceColor = [replaceAll](std::string& str, std::string color, CONSOLE_COLOR consoleColor) {
		if (str.find(color) != std::string::npos) {
			replaceAll(str, color, ConsoleColorToString(consoleColor) + color);
		}
	};
	replaceColor(str, "FAILED", CONSOLE_COLOR::RED);
	replaceColor(str, "SUCCESS", CONSOLE_COLOR::GREEN);
	replaceColor(str, "RED", CONSOLE_COLOR::RED);
	replaceColor(str, "YELLOW", CONSOLE_COLOR::YELLOW);
	replaceColor(str, "GREEN", CONSOLE_COLOR::GREEN);
	replaceColor(str, "BLUE", CONSOLE_COLOR::BLUE);
	replaceColor(str, "MAGENTA", CONSOLE_COLOR::MAGENTA);
	replaceColor(str, "CYAN", CONSOLE_COLOR::CYAN);
	replaceColor(str, "WHITE", CONSOLE_COLOR::WHITE);
	return str;
}

class PRINT_ID : public TreeNode
{
public:
	PRINT_ID(TreeNode* id) : id(id) {}
	TreeNode* id;
public:
	virtual void print() override { std::cout << "print: "; id->print(); }
	virtual void eval(RuntimeGlobals* globals) override {
		id->eval(globals);
		std::string id_name = reinterpret_cast<ID*>(id)->str;
		StackVariable var = globals->pop_var();
		std::cout << "Simon Says: " << id_name << "\t= ";
		if (var.type == VariableType::INTEGER) {
			std::cout << var.intValue;
		}
		else if (var.type == VariableType::STRING) {
			std::cout << "\'" << ColorizeString(var.strValue);
			ResetConsoleColor();
			std::cout << "\'";
		}
		else if (var.type == VariableType::FLOAT) {
			std::cout << std::fixed << std::setprecision(2) << var.fltValue;
		}
		std::cout << "\n";
	}
};

class PRINT_STR : public TreeNode
{
public:
	PRINT_STR(TreeNode* id) : id(id) {}
	TreeNode* id;
public:
	virtual void print() override { std::cout << "print: "; id->print(); }
	virtual void eval(RuntimeGlobals* globals) override {
		id->eval(globals);
		std::string str = globals->pop_var().strValue;
		std::cout << "Simon Says: \'" << ColorizeString(str);
		ResetConsoleColor();
		std::cout << "\'\n";
	}

private:
	
};

class PRINT_DAY : public TreeNode
{
public:
	PRINT_DAY() {}
public:
	virtual void print() override { std::cout << "print: DAY"; }
	virtual void eval(RuntimeGlobals* globals) override {
		std::cout << "Simon Says Todays input is {\n";
		if (globals->DayString.length() == 0) { RuntimeError("Day input not loaded before access!"); }
		std::cout << globals->DayString << "\n}" << std::endl;
	}
};

class LOAD : public TreeNode
{
public:
	LOAD(TreeNode* str) : str(str) {}
	TreeNode* str;
public:
	virtual void print() override { std::cout << "load: "; str->print(); }
	virtual void eval(RuntimeGlobals* globals) override;
};

class EQUALS : public TreeNode
{
public:
	EQUALS(TreeNode* id, TreeNode* expression) : id(id), expression(expression) {}
	TreeNode* id;
	TreeNode* expression;
public:
	virtual void print() override { id->print(); std::cout << " = "; expression->print(); }
	virtual void eval(RuntimeGlobals* globals) override
	{
		std::string id_name = reinterpret_cast<ID*>(id)->str;
		expression->eval(globals);
		StackVariable var = globals->pop_var();
		globals->variables[id_name] = var;
		globals->push_var(var);
	}
};

class IF : public TreeNode
{
public:
	IF(TreeNode* condition, std::vector<TreeNode*> statements, std::vector<TreeNode*> else_statements) : condition(condition), statements(statements), else_statements(else_statements) {}
	TreeNode* condition;
	std::vector<TreeNode*> statements;
	std::vector<TreeNode*> else_statements;
public:
	virtual void print() override {
		std::cout << "IF "; condition->print(); std::cout << " : \n";
		for (auto statment : statements)
		{
			statment->print();
		}
		std::cout << "ELSE : ";
		for (auto else_statment : else_statements)
		{
			else_statment->print();
		}
		std::cout << "END";

	}
	virtual void eval(RuntimeGlobals* globals) override
	{
		condition->eval(globals);
		int condition_value = globals->pop_var().intValue;
		if (condition_value != 0)
		{
			for (auto statment : statements)
			{
				statment->eval(globals);
			}
		}
		else
		{
			for (auto else_statment : else_statements)
			{
				else_statment->eval(globals);
			}
		}
	}
};

class ASSERT : public TreeNode
{
public:
	ASSERT(TreeNode* condition, TreeNode* str) : condition(condition), str(str) {}
	TreeNode* condition;
	TreeNode* str;
public:
	virtual void print() override {
		std::cout << "ASSERT ("; 
		condition->print(); 
		std::cout << ") : \'"; 
		str->print();
		std::cout << "\'";
	}

	virtual void eval(RuntimeGlobals* globals) override
	{
		condition->eval(globals);
		int condition_value = globals->pop_var().intValue;
		if (condition_value == 0)
		{
			str->eval(globals);
			std::string str_value = globals->pop_var().strValue;

			PushConsoleColor(CONSOLE_COLOR::BLUE);
			std::cout << "Assert condition: ( "; condition->print(); std::cout << " )\n";
			PopConsoleColor();

			throw std::invalid_argument("ASSERT FAILED!: " + str_value);
		}
	}
};

class LOOP : public TreeNode
{
public:
	LOOP(TreeNode* times, std::vector<TreeNode*> statements) : times(times), statements(statements) {}
	TreeNode* times;
	std::vector<TreeNode*> statements;
public:
	virtual void print() override {
		std::cout << "LOOP "; times->print(); std::cout << " TIMES : \n";
		for (auto statment : statements)
		{
			statment->print();
		}
		std::cout << "LOOPEND";

	}
	virtual void eval(RuntimeGlobals* globals) override
	{
		times->eval(globals);
		int times_value = globals->pop_var().intValue;
		for (int i = 0; i < times_value; ++i)
		{
			for (auto statment : statements)
			{
				statment->eval(globals);
			}
		}
	}
};

class STRING : public TreeNode
{
public:
	STRING(std::string str) : str(str) {}
	std::string str;
public:
	virtual void print() override { std::cout << str; }
	virtual void eval(RuntimeGlobals* globals) override
	{
		globals->push_var(str);
	}
};

class INTEGER : public TreeNode
{
public:
	INTEGER(int num) : num(num) {}
	int num;
public:
	virtual void print() override { std::cout << num; }
	virtual void eval(RuntimeGlobals* globals) override
	{
		globals->push_var(num);
	}
};

class FLOAT : public TreeNode
{
public:
	FLOAT(float num) : num(num) {}
	float num;
public:
	virtual void print() override { std::cout << num; }
	virtual void eval(RuntimeGlobals* globals) override
	{
		globals->push_var(num);
	}
};

class Parser
{
//	See grammar in README.md 
public:
	Parser(std::string code, bool printSyntax);
	~Parser();
	TreeNode* getAST() { return ast; };
private:
	bool ScanExpression(Token t, TreeNode** outNode);
	bool ScanLogic(Token t, TreeNode** outNode);
	bool ScanTerm(Token t, TreeNode** outNode);
	bool ScanFactor(Token t, TreeNode** outNode);
	bool ScanNegate(Token t, TreeNode** outNode);
	bool ScanAssignment(Token t, TreeNode** outNode);
	bool ScanID(Token t, TreeNode** outNode);
	bool ScanString(Token t, TreeNode** outNode);
	bool ScanPrint(Token t, TreeNode** outNode);
	bool ScanLoad(Token t, TreeNode** outNode);
	bool ScanIf(Token t, TreeNode** outNode);
	bool ScanLoop(Token t, TreeNode** outNode);
	bool ScanAssert(Token t, TreeNode** outNode);
	bool ScanStatement(Token t, TreeNode** outNode, bool programStatement = true);

	RuntimeGlobals globals;
	Tokenizer tokenizer;
	TreeNode* ast;
	std::vector<TreeNode*> nodes;
	std::vector<TreeNode*> statements;
};