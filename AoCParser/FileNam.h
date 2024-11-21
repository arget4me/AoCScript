#pragma once

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