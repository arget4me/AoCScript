#pragma once
#include "Tokenizer.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>

static std::vector<uint8_t*> stack = {};
static std::map<std::string, int> variables = {};

static int pop_int() {
	auto out = stack.back();
	stack.pop_back();
	int result = *reinterpret_cast<int*>(out);
	delete out;
	return result;
}

static std::string pop_str() {
	auto out = stack.back();
	stack.pop_back();
	std::string result = *reinterpret_cast<std::string*>(out);
	delete out;
	return result;
}

static void push_num(int num) {
	stack.push_back(reinterpret_cast<uint8_t*>(new int(num)));
}

static void push_str(std::string str) {
	stack.push_back(reinterpret_cast<uint8_t*>(new std::string(str)));
}

static void pop() {
	if (stack.size() == 0) return;
	delete stack.back();
	stack.pop_back();
}

class TreeNode
{
public:
	virtual void print() = 0;
	virtual void eval() = 0;
};

class Statement : public TreeNode
{
public:
	Statement(TreeNode* statement) : statement(statement) {}
	TreeNode* statement;
	virtual void print() override {
		statement->print(); std::cout << ";\n";
	}

	virtual void eval() override {
		statement->eval();
		pop();
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
	virtual void eval() override {
		left->eval();
		int left = pop_int();

		right->eval();
		int right = pop_int();
		
		int result = left + right;
		push_num(result);
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
	virtual void eval() override {
		left->eval();
		int left = pop_int();

		right->eval();
		int right = pop_int();

		int result = left - right;
		push_num(result);
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
	virtual void eval() override {
		left->eval();
		int left = pop_int();

		right->eval();
		int right = pop_int();

		int result = left * right;
		push_num(result);
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
	virtual void eval() override {
		left->eval();
		int left = pop_int();

		right->eval();
		int right = pop_int();

		int result = left / right;
		push_num(result);
	}
};

class NEGATE : public TreeNode
{
public:
	TreeNode* arg;
public:
	virtual void print() override {
		std::cout << "(";
		std::cout << "-"; arg->print();
		std::cout << ")";
	}
	virtual void eval() override { std::cout << "NEGATE NOT IMPLEMENTED "; }
};

class ID : public TreeNode
{
public:
	ID(std::string str) : str(str) {}
	std::string str;
public:
	virtual void print() override { std::cout << str; }
	virtual void eval() override 
	{
		auto found = variables.find(str);
		if (found != variables.end()) {
			push_num(variables[str]);
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
	virtual void eval() override
	{
		push_str(str);
	}
};


class PRINT : public TreeNode
{
public:
	PRINT(TreeNode* id) : id(id) {}
	TreeNode* id;
public:
	virtual void print() override { std::cout << "print: "; id->print(); }
	virtual void eval() override {
		id->eval();
		std::string id_name = reinterpret_cast<ID*>(id)->str;
		int result = pop_int();
		std::cout << "Simon Says: " << id_name << "\t= " << result << "\n";
		push_num(result);
	}
};

class LOAD : public TreeNode
{
public:
	LOAD(TreeNode* str) : str(str) {}
	TreeNode* str;
public:
	virtual void print() override { std::cout << "load: "; str->print(); }
	virtual void eval() override {
		str->eval();
		std::string load_file = pop_str();
		variables["DAY"] = 1337;
		push_num(variables["DAY"]);
	}
};


class EQUALS : public TreeNode
{
public:
	EQUALS(TreeNode* id, TreeNode* expression) : id(id), expression(expression) {}
	TreeNode* id;
	TreeNode* expression;
public:
	virtual void print() override { id->print(); std::cout << " = "; expression->print(); }
	virtual void eval() override
	{
		std::string id_name = reinterpret_cast<ID*>(id)->str;
		expression->eval();
		int right = pop_int();
		variables[id_name] = right;
		push_num(right);
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
	virtual void eval() override
	{
		condition->eval();
		int condition_value = pop_int();
		if (condition_value != 0)
		{
			for (auto statment : statements)
			{
				statment->eval();
			}
		}
		else
		{
			for (auto else_statment : else_statements)
			{
				else_statment->eval();
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
	virtual void eval() override
	{
		push_num(num);
	}
};

class Parser
{
/* =======================================================================================================
											GRAMMAR
   =======================================================================================================
		Program				::= {Statement}
		Statement			::= Assignment ";"
								| PrintStatement ";"
								| LoadStatement ";"
								| IfStatement ";"
		Assignment			::= Identifier "=" Expression
		PrintStatement		::= "print" | "simon says" Identifier
		LoadStatement		::= "load" String
		IfStatement			::= "if" Expression ":" {Statement} "else" ":" {Statement} "end"
		Expression			::= Term { ("+" | "-") Term}
		Term				::= Factor { ("*" | "/") Factor}
		Factor				::= Number
								| Identifier
		Identifier			::= Letter { Letter | Digit }
		Number				::= Digit { Digit }
		Letter				::=	"a" | ... |"z" | "A" | ... | "Z"    // Any alphabetical character, [a-zA-Z]
		Digit				::= "0" | ... | "9"						// Any numeric digit [0-9]
		String				::= \".*\"
   ======================================================================================================= */

public:
	Parser(std::string code);
	~Parser();
	TreeNode* getAST() { return ast; };

private:
	bool ScanExpression(Token t, TreeNode** outNode);
	bool ScanTerm(Token t, TreeNode** outNode);
	bool ScanFactor(Token t, TreeNode** outNode);
	bool ScanAssignment(Token t, TreeNode** outNode);
	bool ScanID(Token t, TreeNode** outNode);
	bool ScanString(Token t, TreeNode** outNode);
	bool ScanPrint(Token t, TreeNode** outNode);
	bool ScanLoad(Token t, TreeNode** outNode);
	bool ScanIf(Token t, TreeNode** outNode);
	bool ScanStatement(Token t, TreeNode** outNode, bool programStatement = true);

	Tokenizer tokenizer;
	TreeNode* ast;
	std::vector<TreeNode*> nodes;
	std::vector<TreeNode*> statements;
};