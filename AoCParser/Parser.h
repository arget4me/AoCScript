#pragma once
#include "Tokenizer.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>

bool ReadFile(const std::string& filePath, std::string& fileContents);

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
		for (auto* stack_ptr : stack) {
			delete stack_ptr;
		}
	}

	std::vector<uint8_t*> stack;
	std::map<std::string, int> variables;

	std::vector<std::string> DayLines;
	std::string DayString;
	std::string DayFileName;

	int pop_int();
	std::string pop_str();

	void push_int(int num);
	void push_str(std::string str);

	void pop();
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
		globals->pop();
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
		int left = globals->pop_int();

		right->eval(globals);
		int right = globals->pop_int();
		
		int result = left + right;
		globals->push_int(result);
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
		int left = globals->pop_int();

		right->eval(globals);
		int right = globals->pop_int();

		int result = left - right;
		globals->push_int(result);
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
		int left = globals->pop_int();

		right->eval(globals);
		int right = globals->pop_int();

		int result = left * right;
		globals->push_int(result);
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
		int left = globals->pop_int();

		right->eval(globals);
		int right = globals->pop_int();

		int result = left / right;
		globals->push_int(result);
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
		int left = globals->pop_int();

		right->eval(globals);
		int right = globals->pop_int();

		int result = left % right;
		globals->push_int(result);
	}
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
		int left = globals->pop_int();

		right->eval(globals);
		int right = globals->pop_int();

		int result = left > right;
		globals->push_int(result);
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
		int left = globals->pop_int();

		right->eval(globals);
		int right = globals->pop_int();

		int result = left >= right;
		globals->push_int(result);
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
		int left = globals->pop_int();

		right->eval(globals);
		int right = globals->pop_int();

		int result = left < right;
		globals->push_int(result);
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
		int left = globals->pop_int();

		right->eval(globals);
		int right = globals->pop_int();

		int result = left <= right;
		globals->push_int(result);
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
		int left = globals->pop_int();

		right->eval(globals);
		int right = globals->pop_int();

		int result = left == right;
		globals->push_int(result);
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
		int arg_value = globals->pop_int();
		globals->push_int(-arg_value);
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
			globals->push_int(globals->variables[str]);
		}
		else {
			// TODO Throw runtime error
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
		globals->push_str(str);
	}
};


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
		int result = globals->pop_int();
		std::cout << "Simon Says: " << id_name << "\t= " << result << "\n";
		globals->push_int(result);
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
		std::string str = globals->pop_str();
		std::cout << "Simon Says: \'" << str << "\'\n";
		globals->push_int(0);
	}
};

class PRINT_DAY : public TreeNode
{
public:
	PRINT_DAY() {}
public:
	virtual void print() override { std::cout << "print: DAY"; }
	virtual void eval(RuntimeGlobals* globals) override {
		std::cout << "Simon Says: \"Todays input is:\"\n";
		std::cout << globals->DayString << std::endl;
		globals->push_int(0);
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
		int right = globals->pop_int();
		globals->variables[id_name] = right;
		globals->push_int(right);
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
		int condition_value = globals->pop_int();
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
		int condition_value = globals->pop_int();
		if (condition_value == 0)
		{
			str->eval(globals);
			std::string str_value = globals->pop_str();
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
		int times_value = globals->pop_int();
		for (int i = 0; i < times_value; ++i)
		{
			for (auto statment : statements)
			{
				statment->eval(globals);
			}
		}
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
		globals->push_int(num);
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