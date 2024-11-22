#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "Tokenizer.h"

class TreeNode
{
public:
	virtual void print() = 0;
};

class ADD : public TreeNode
{
public:
	TreeNode* left;
	TreeNode* right;
public:
	virtual void print() {
		std::cout << "(";
		left->print(); std::cout << " + "; right->print();
		std::cout << ")";
	}
};

class SUBTRACT : public TreeNode
{
public:
	TreeNode* left;
	TreeNode* right;
public:
	virtual void print() {
		std::cout << "(";
		left->print(); std::cout << " - "; right->print();
		std::cout << ")";
	}
};

class MULT : public TreeNode
{
public:
	TreeNode* left;
	TreeNode* right;
public:
	virtual void print() {
		std::cout << "(";
		left->print(); std::cout << " * "; right->print();
		std::cout << ")";
	}
};

class DIV : public TreeNode
{
public:
	TreeNode* left;
	TreeNode* right;
public:
	virtual void print() {
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
	virtual void print() {
		std::cout << "(";
		std::cout << "-"; arg->print();
		std::cout << ")";
	}
};

class ID : public TreeNode
{
public:
	std::string str;
public:
	virtual void print() { std::cout << str; }
};

class INTEGER : public TreeNode
{
public:
	int num;
public:
	virtual void print() { std::cout << num; }
};

class Parser
{
/* =======================================================================================================
											GRAMMAR
   =======================================================================================================
		Program				::= {Statement}
		Statement			::= Assignment ";"
								| PrintStatement ";"
		Assignment			::= Identifier "=" Expression
		PrintStatement		::= "print" Expression
		Expression			::= Term { ("+" | "-") Term}
		Term				::= Factor { ("*" | "/") Factor}
		Factor				::= Number
								| Identifier
								| "(" Expression ")"
		Identifier			::= Letter { Letter | Digit }
		Number				::= Digit { Digit }
		Letter				::=	"a" | ... |"z" | "A" | ... | "Z"    // Any alphabetical character, [a-zA-Z]
		Digit				::= "0" | ... | "9"						// Any numeric digit [0-9]
   ======================================================================================================= */

public:
	Parser(std::string code);
	~Parser();
	TreeNode* getAST() { return ast; };

private:
	Tokenizer tokenizer;
	TreeNode* ast;
	std::vector<TreeNode*> nodes;
};