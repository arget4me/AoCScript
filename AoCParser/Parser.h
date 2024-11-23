#pragma once
#include "Tokenizer.h"
#include <string>
#include <iostream>
#include <vector>

class TreeNode
{
public:
	virtual void print() = 0;
};

class Statement : public TreeNode
{
public:
	Statement(TreeNode* statement) : statement(statement) {}
	TreeNode* statement;
	virtual void print() override {
		statement->print(); std::cout << ";\n";
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
};

class ID : public TreeNode
{
public:
	ID(std::string str) : str(str) {}
	std::string str;
public:
	virtual void print() override { std::cout << str; }
};

class STRING : public TreeNode
{
public:
	STRING(std::string str) : str(str) {}
	std::string str;
public:
	virtual void print() override { std::cout << str; }
};


class PRINT : public TreeNode
{
public:
	PRINT(TreeNode* id) : id(id) {}
	TreeNode* id;
public:
	virtual void print() override { std::cout << "print: "; id->print(); }
};

class LOAD : public TreeNode
{
public:
	LOAD(TreeNode* str) : str(str) {}
	TreeNode* str;
public:
	virtual void print() override { std::cout << "load: "; str->print(); }
};


class EQUALS : public TreeNode
{
public:
	EQUALS(TreeNode* id, TreeNode* expression) : id(id), expression(expression) {}
	TreeNode* id;
	TreeNode* expression;
public:
	virtual void print() override { id->print(); std::cout << " = "; expression->print(); }
};


class INTEGER : public TreeNode
{
public:
	INTEGER(int num) : num(num) {}
	int num;
public:
	virtual void print() override { std::cout << num; }
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
		Assignment			::= Identifier "=" Expression
		PrintStatement		::= "print" | "simon says" Identifier
		LoadStatement		::= "load" String
		Expression			::= Term { ("+" | "-") Term}
		Term				::= Number
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
	bool ScanAssignment(Token t, TreeNode** outNode);
	bool ScanID(Token t, TreeNode** outNode);
	bool ScanString(Token t, TreeNode** outNode);
	bool ScanPrint(Token t, TreeNode** outNode);
	bool ScanLoad(Token t, TreeNode** outNode);
	bool ScanStatement(Token t, TreeNode** outNode);

	Tokenizer tokenizer;
	TreeNode* ast;
	std::vector<TreeNode*> nodes;
	std::vector<TreeNode*> statements;
};