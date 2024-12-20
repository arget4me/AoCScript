#pragma once
#include "Tokenizer.h"
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <iomanip> // For manipulators : std::setprecision(2)
#include "PrintHelper.h"

bool ReadFile(const std::string& filePath, std::string& fileContents);
void SyntaxError(Tokenizer& tokenizer, Token token, std::string expected);
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

	int GetSortPrio() const {
		switch (type)
		{
		case VariableType::INTEGER:
			return intValue;
			break;
		case VariableType::STRING:
			return static_cast<int>(strValue.length());
			break;
		case VariableType::FLOAT:
			return static_cast<int>(fltValue);
			break;
		}
		return 0;
	}

	// Overload the < operator for sorting MyClass objects
	bool operator<(const StackVariable& other) const {
		if (type == VariableType::STRING && other.type == VariableType::STRING)
		{
			return strValue < other.strValue;
		}

		return GetSortPrio() < other.GetSortPrio();
	}
};

struct List
{
	List(VariableType type) : type(type) {}
	VariableType type;
	std::vector<StackVariable> list;

	virtual void push_var(StackVariable var);
	virtual StackVariable pop_var();
	virtual void set_var(int index, StackVariable expressionVar);
};

struct SortedList : List
{
	SortedList(VariableType type) : List(type) {}
	virtual void push_var(StackVariable var) override;
	virtual void set_var(int index, StackVariable expressionVar) override;
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
		breakCounter = 0;
	}

	~RuntimeGlobals() {
		for (auto& pair : lists)
		{
			delete pair.second;
		}
	}	

	std::vector<StackVariable> stack;
	std::map<std::string, StackVariable> variables;
	std::map<std::string, List*> lists;

	std::vector<std::string> DayLines;
	std::string DayString;
	std::string DayFileName;

	void push_var(int var) { push_var(StackVariable(var)); };
	void push_var(std::string var) { push_var(StackVariable(var)); };
	void push_var(float var) { push_var(StackVariable(var)); };
	void push_var(StackVariable var);
	StackVariable pop_var();


	void push_break() { ++breakCounter; };
	bool pop_break() {
		if (breakCounter > 0) {
			--breakCounter; 
			return true;
		}
		return false;
	}
private:
	int breakCounter;
};

class TreeNode
{
public:
	virtual ~TreeNode() = default;

	virtual void print() = 0;
	virtual void eval(RuntimeGlobals* globals) = 0;
};

class Statement : public TreeNode
{
public:
	Statement(TreeNode* statement) : statement(statement) {}
	virtual ~Statement() override = default;
	TreeNode* statement;
	virtual void print() override {
		statement->print(); std::cout << ";\n";
	}

	virtual void eval(RuntimeGlobals* globals) override {
		statement->eval(globals);
		globals->pop_var();
	}
};

class IS_OPERATOR : public TreeNode
{
public:
	IS_OPERATOR(TreeNode* left) : left(left) {}
	virtual ~IS_OPERATOR() override = default;
	TreeNode* left;
};

class IS_DIGIT : public IS_OPERATOR
{
public:
	IS_DIGIT(TreeNode* left) : IS_OPERATOR(left) {}
	virtual ~IS_DIGIT() override = default;
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " IS DIGIT";
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		StackVariable var = globals->pop_var();

		if (var.type == VariableType::STRING) {
			bool isDigit = var.strValue.size() > 0;
			for (char& c : var.strValue) {
				if (!std::isdigit(c)) {
					isDigit = false;
					break;
				}
			}
			globals->push_var(static_cast<int>(isDigit));
		}
		else {
			// The other types are already known to be digits
			globals->push_var(1);
		}
	}
};

class IS_ALPHA : public IS_OPERATOR
{
public:
	IS_ALPHA(TreeNode* left) : IS_OPERATOR(left) {}
	virtual ~IS_ALPHA() override = default;

	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " IS ALPHA";
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		StackVariable var = globals->pop_var();

		if (var.type == VariableType::STRING) {
			bool isAlpha = var.strValue.size() > 0;
			for (char& c : var.strValue) {
				if (!std::isalpha(c)) {
					isAlpha = false;
					break;
				}
			}
			globals->push_var(static_cast<int>(isAlpha));
		}
		else {
			// The other types are already know to not be alpha
			globals->push_var(0);
		}
	}
};

class OPERATOR : public TreeNode
{
public:
	OPERATOR(TreeNode* left, TreeNode* right) : left(left), right(right) {}
	virtual ~OPERATOR() override = default;
	TreeNode* left;
	TreeNode* right;
};


class ADD : public OPERATOR
{
public:
	ADD(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual ~ADD() override = default;
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " + "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		StackVariable left_var = globals->pop_var();

		right->eval(globals);
		StackVariable right_var = globals->pop_var();

		if (left_var.type != right_var.type) {
			RuntimeError("Type mismatch: " + VariableTypeToString(left_var.type) + " + " + VariableTypeToString(right_var.type));
		}

		switch (left_var.type)
		{
		case VariableType::INTEGER:
			globals->push_var(left_var.intValue + right_var.intValue);
			break;
		case VariableType::STRING:
			globals->push_var(left_var.strValue + right_var.strValue);
			break;
		case VariableType::FLOAT:
			globals->push_var(left_var.fltValue + right_var.fltValue);
			break;
		}		
	}
};

class SUBTRACT : public OPERATOR
{
public:
	SUBTRACT(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual ~SUBTRACT() override = default;
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
	virtual ~MULT() override = default;
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
	virtual ~DIV() override = default;
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
	virtual ~MODULO() override = default;
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
	virtual ~CAST() override = default;
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
	virtual ~GREATER_THAN() override = default;
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " > "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		StackVariable left_var = globals->pop_var();

		right->eval(globals);
		StackVariable right_var = globals->pop_var();

		if (left_var.type != right_var.type) {
			RuntimeError("Type mismatch: " + VariableTypeToString(left_var.type) + " > " + VariableTypeToString(right_var.type));
		}

		switch (left_var.type)
		{
		case VariableType::INTEGER:
			globals->push_var(static_cast<int>(left_var.intValue > right_var.intValue));
			break;
		case VariableType::STRING:
			globals->push_var(static_cast<int>(left_var.strValue > right_var.strValue));
			break;
		case VariableType::FLOAT:
			globals->push_var(static_cast<int>(left_var.fltValue > right_var.fltValue));
			break;
		}
	}
};

class GREATER_EQUALS : public OPERATOR
{
public:
	GREATER_EQUALS(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual ~GREATER_EQUALS() override = default;
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " >= "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		StackVariable left_var = globals->pop_var();

		right->eval(globals);
		StackVariable right_var = globals->pop_var();

		if (left_var.type != right_var.type) {
			RuntimeError("Type mismatch: " + VariableTypeToString(left_var.type) + " >= " + VariableTypeToString(right_var.type));
		}

		switch (left_var.type)
		{
		case VariableType::INTEGER:
			globals->push_var(static_cast<int>(left_var.intValue >= right_var.intValue));
			break;
		case VariableType::STRING:
			globals->push_var(static_cast<int>(left_var.strValue >= right_var.strValue));
			break;
		case VariableType::FLOAT:
			globals->push_var(static_cast<int>(left_var.fltValue >= right_var.fltValue));
			break;
		}
	}
};

class LESS_THAN : public OPERATOR
{
public:
	LESS_THAN(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual ~LESS_THAN() override = default;
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " < "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		StackVariable left_var = globals->pop_var();

		right->eval(globals);
		StackVariable right_var = globals->pop_var();

		if (left_var.type != right_var.type) {
			RuntimeError("Type mismatch: " + VariableTypeToString(left_var.type) + " < " + VariableTypeToString(right_var.type));
		}

		switch (left_var.type)
		{
		case VariableType::INTEGER:
			globals->push_var(static_cast<int>(left_var.intValue < right_var.intValue));
			break;
		case VariableType::STRING:
			globals->push_var(static_cast<int>(left_var.strValue < right_var.strValue));
			break;
		case VariableType::FLOAT:
			globals->push_var(static_cast<int>(left_var.fltValue < right_var.fltValue));
			break;
		}
	}
};

class LESS_EQUALS : public OPERATOR
{
public:
	LESS_EQUALS(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual ~LESS_EQUALS() override = default;
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " <= "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		StackVariable left_var = globals->pop_var();

		right->eval(globals);
		StackVariable right_var = globals->pop_var();

		if (left_var.type != right_var.type) {
			RuntimeError("Type mismatch: " + VariableTypeToString(left_var.type) + " <= " + VariableTypeToString(right_var.type));
		}

		switch (left_var.type)
		{
		case VariableType::INTEGER:
			globals->push_var(static_cast<int>(left_var.intValue <= right_var.intValue));
			break;
		case VariableType::STRING:
			globals->push_var(static_cast<int>(left_var.strValue <= right_var.strValue));
			break;
		case VariableType::FLOAT:
			globals->push_var(static_cast<int>(left_var.fltValue <= right_var.fltValue));
			break;
		}
	}
};

class IS_EQUAL : public OPERATOR
{
public:
	IS_EQUAL(TreeNode* left, TreeNode* right) : OPERATOR(left, right) {}
	virtual ~IS_EQUAL() override = default;
	virtual void print() override {
		std::cout << "(";
		left->print(); std::cout << " == "; right->print();
		std::cout << ")";
	}
	virtual void eval(RuntimeGlobals* globals) override {
		left->eval(globals);
		StackVariable left_var = globals->pop_var();

		right->eval(globals);
		StackVariable right_var = globals->pop_var();

		if (left_var.type != right_var.type) {
			RuntimeError("Type mismatch: " + VariableTypeToString(left_var.type) + " == " + VariableTypeToString(right_var.type));
		}

		switch (left_var.type)
		{
		case VariableType::INTEGER:
			globals->push_var(static_cast<int>(left_var.intValue == right_var.intValue));
			break;
		case VariableType::STRING:
			globals->push_var(static_cast<int>(left_var.strValue == right_var.strValue));
			break;
		case VariableType::FLOAT:
			globals->push_var(static_cast<int>(left_var.fltValue == right_var.fltValue));
			break;
		}
	}
};

class NEGATE : public TreeNode
{
public:
	NEGATE(TreeNode* arg) :arg(arg) {}
	virtual ~NEGATE() override = default;
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
	virtual ~ID() override = default;
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
			RuntimeError("Identifier " + str + " does not exist!");
		}
	}
};

class ARRAY_SIZE : public TreeNode
{
public:
	ARRAY_SIZE(TreeNode* id) :id(id) {}
	virtual ~ARRAY_SIZE() override = default;
	TreeNode* id;

	virtual void print() override {
		std::cout << "(";
		id->print();
		std::cout << " SIZE";
		std::cout << ")";
	}

	virtual void eval(RuntimeGlobals* globals) override {
		std::string id_name = reinterpret_cast<ID*>(id)->str;
		if (globals->lists.find(id_name) != globals->lists.end())
		{
			globals->push_var(static_cast<int>(globals->lists[id_name]->list.size()));
		}
		else {
			id->eval(globals);
			StackVariable var = globals->pop_var();
			if (var.type == VariableType::STRING) {
				globals->push_var(static_cast<int>(var.strValue.length()));
			}
			else {
				RuntimeError("Variable of type " + VariableTypeToString(var.type) + " can't be indexed.");
			}
		}
	}
};

class ARRAY_INDEXING : public TreeNode
{
public:
	ARRAY_INDEXING(TreeNode* id, TreeNode* expression) :id(id), expression(expression) {}
	virtual ~ARRAY_INDEXING() override = default;
	TreeNode* id;
	TreeNode* expression;

	virtual void print() override {
		std::cout << "(";
		id->print();
		std::cout << "["; expression->print(); std::cout << "]";
		std::cout << ")";
	}

	virtual void eval(RuntimeGlobals* globals) override {
		std::string id_name = reinterpret_cast<ID*>(id)->str;
		expression->eval(globals);
		StackVariable varIndex = globals->pop_var();
		if (varIndex.type != VariableType::INTEGER) {
			RuntimeError("Can't index array " + id_name + " with index of type " + VariableTypeToString(varIndex.type) + ". Only INTEGER indices are allowed.");
		}

		int index = varIndex.intValue;
		if (index < 0) {
			RuntimeError("Array index must be possitive: " + index);
		}

		if (globals->lists.find(id_name) != globals->lists.end())
		{
			List* list = globals->lists[id_name];
			if (index >= list->list.size()) {
				RuntimeError("Array index out of range: " + std::to_string(index)
					+ ". Size = " + std::to_string(list->list.size()));
			}

			// VariableType is handled by push_var
			globals->push_var(list->list[index]);
		}
		else {
			id->eval(globals);
			StackVariable var = globals->pop_var();
			if (var.type == VariableType::STRING) {
				if (index >= var.strValue.length()) {
					RuntimeError("Array index out of range: " + std::to_string(index)
						+ ". Size = " + std::to_string(var.strValue.length()));
				}

				// Push it as a single string character instead of casting to an int.
				globals->push_var(std::string(1, var.strValue[index]));
			}
			else {
				RuntimeError("Variable of type " + VariableTypeToString(var.type) + " can't be indexed.");
			}
		}
	}
};

class EQUALS_INDEXED : public TreeNode
{
public:
	EQUALS_INDEXED(TreeNode* id, TreeNode* index, TreeNode* expression) : id(id), index(index), expression(expression) {}
	virtual ~EQUALS_INDEXED() override = default;
	TreeNode* id;
	TreeNode* index;
	TreeNode* expression;
public:
	virtual void print() override { id->print(); std::cout << " = "; expression->print(); }
	virtual void eval(RuntimeGlobals* globals) override
	{
		std::string id_name = reinterpret_cast<ID*>(id)->str;

		index->eval(globals);
		StackVariable varIndex = globals->pop_var();
		if (varIndex.type != VariableType::INTEGER) {
			RuntimeError("Can't index array " + id_name + " with index of type " + VariableTypeToString(varIndex.type) + ". Only INTEGER indices are allowed.");
		}

		int index = varIndex.intValue;
		if (index < 0) {
			RuntimeError("Array index must be possitive: " + index);
		}

		expression->eval(globals);
		StackVariable expressionVar = globals->pop_var();

		if (globals->lists.find(id_name) != globals->lists.end())
		{
			List* list = globals->lists[id_name];
			if (index >= list->list.size()) {
				RuntimeError("Array index out of range: " + std::to_string(index)
					+ ". Size = " + std::to_string(list->list.size()));
			}

			if (expressionVar.type != list->type)
			{
				RuntimeError("Can't add value of type {" + VariableTypeToString(expressionVar.type) + "} to list "
					+ id_name + "<" + VariableTypeToString(list->type) + ">");
			}

			list->set_var(index, expressionVar);
		}
		else {
			id->eval(globals);
			StackVariable idVar = globals->pop_var();
			if (idVar.type == VariableType::STRING) {
				if (index >= idVar.strValue.length()) {
					RuntimeError("Array index out of range: " + std::to_string(index)
						+ ". Size = " + std::to_string(idVar.strValue.length()));
				}

				idVar.strValue[index] = expressionVar.strValue[0];
				globals->variables[id_name] = idVar;
			}
			else {
				RuntimeError("Variable of type " + VariableTypeToString(idVar.type) + " can't be indexed.");
			}
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
	virtual ~PRINT_ID() override = default;
	TreeNode* id;
public:
	virtual void print() override { std::cout << "print: "; id->print(); }
	virtual void eval(RuntimeGlobals* globals) override {
		
		std::string id_name = reinterpret_cast<ID*>(id)->str;
		std::cout << "Simon Says: " << id_name << "\t= ";

		if (globals->lists.find(id_name) != globals->lists.end())
		{
			std::cout << "[ ";
			List* list = globals->lists[id_name];
			bool first = true;
			for (StackVariable& var : list->list)
			{
				if (!first) {
					std::cout << ", ";
				}
				else {
					first = false;
				}

				switch (list->type)
				{
				case VariableType::INTEGER:
					std::cout << var.intValue;
					break;
				case VariableType::STRING:
					std::cout << var.strValue;
					break;
				case VariableType::FLOAT:
					std::cout << var.fltValue;
					break;
				default:
					break;
				}
			}
			std::cout << " ]";
		}
		else {
			id->eval(globals);
			StackVariable var = globals->pop_var();
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
		}
		std::cout << "\n";
	}
};

class PRINT_STR : public TreeNode
{
public:
	PRINT_STR(TreeNode* id) : id(id) {}
	virtual ~PRINT_STR() override = default;
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
	virtual ~PRINT_DAY() override = default;
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
	virtual ~LOAD() override = default;
	TreeNode* str;
public:
	virtual void print() override { std::cout << "load: "; str->print(); }
	virtual void eval(RuntimeGlobals* globals) override;
};

class EQUALS : public TreeNode
{
public:
	EQUALS(TreeNode* id, TreeNode* expression) : id(id), expression(expression) {}
	virtual ~EQUALS() override = default;
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

class LIST_CREATE : public TreeNode
{
public:
	LIST_CREATE(TreeNode* id, bool sorted, VariableType type)
		: id(id), sorted(sorted), type(type) {}
	virtual ~LIST_CREATE() override = default;
	TreeNode* id;
	bool sorted;
	VariableType type;
public:
	virtual void print() override { 
		std::cout << "CREATE LIST<" << VariableTypeToString(type) << "> ( ";
		id->print();
		std::cout << " )";
	}
	virtual void eval(RuntimeGlobals* globals) override
	{
		std::string id_name = reinterpret_cast<ID*>(id)->str;
		if (sorted)
		{
			globals->lists[id_name] = new SortedList(type);
		}
		else
		{
			globals->lists[id_name] = new List(type);
		}
	}
};

class LIST_ADD : public TreeNode
{
public:
	LIST_ADD(TreeNode* id, TreeNode* expression)
		: id(id), expression(expression) {}
	virtual ~LIST_ADD() override = default;
	TreeNode* id;
	TreeNode* expression;
public:
	virtual void print() override {
		id->print();
		std::cout << " << ";
		expression->print();
	}
	virtual void eval(RuntimeGlobals* globals) override
	{
		std::string id_name = reinterpret_cast<ID*>(id)->str;
		if (globals->lists.find(id_name) == globals->lists.end())
		{
			RuntimeError("Could not find list '" + id_name + "'");
		}

		List* list = globals->lists[id_name];

		expression->eval(globals);
		StackVariable var = globals->pop_var();

		if (var.type != list->type) 
		{
			RuntimeError("Can't add value of type {" + VariableTypeToString(var.type) + "} to list " 
				+ id_name + "<" + VariableTypeToString(list->type) + ">");
		}

		list->push_var(var);
	}
};

class IF : public TreeNode
{
public:
	IF(TreeNode* condition, std::vector<TreeNode*> statements, std::vector<TreeNode*> else_statements) : condition(condition), statements(statements), else_statements(else_statements) {}
	virtual ~IF() override = default;
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
	virtual ~ASSERT() override = default;
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
	virtual ~LOOP() override = default;
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

		bool doBreak = false;
		int ITER = 0;
		for (int i = 0; i < times_value; ++i)
		{
			for (auto statment : statements)
			{
				globals->variables["ITER"] = ITER;
				statment->eval(globals);
				if (doBreak || globals->pop_break()) { doBreak = true;  break; }
			}
			if (doBreak || globals->pop_break()) { doBreak = true;  break; }

			++ITER;
		}
		globals->variables.erase("ITER");
	}
};

class LOOP_ITERATOR : public TreeNode
{
public:
	LOOP_ITERATOR(TreeNode* id, std::vector<TreeNode*> statements) : id(id), statements(statements) {}
	virtual ~LOOP_ITERATOR() override = default;
	TreeNode* id;
	std::vector<TreeNode*> statements;
public:
	virtual void print() override {
		std::cout << "LOOP "; id->print(); std::cout << " CHARS : \n";
		for (auto statment : statements)
		{
			statment->print();
		}
		std::cout << "LOOPEND";

	}
	virtual void eval(RuntimeGlobals* globals) override
	{
		std::string id_name = reinterpret_cast<ID*>(id)->str;
		if (globals->lists.find(id_name) != globals->lists.end())
		{
			List* list = globals->lists[id_name];
			bool doBreak = false;
			int ITER = 0;
			for (StackVariable& var : list->list)
			{
				switch (list->type)
				{
				case VariableType::INTEGER:
					{
						for (auto statment : statements)
						{
							globals->variables["CHAR"] = StackVariable(var.intValue);
							globals->variables["ITER"] = ITER;
							statment->eval(globals);
							if (doBreak || globals->pop_break()) { doBreak = true;  break; }
						}
					}
					break;
				case VariableType::STRING:
					{
						for (auto statment : statements)
						{
							globals->variables["CHAR"] = StackVariable(var.strValue);
							globals->variables["ITER"] = ITER;
							statment->eval(globals);
							if (doBreak || globals->pop_break()) { doBreak = true;  break; }
						}
					}
					break;
				case VariableType::FLOAT:
					{
						for (auto statment : statements)
						{
							globals->variables["CHAR"] = StackVariable(var.fltValue);
							globals->variables["ITER"] = ITER;
							statment->eval(globals);
							if (doBreak || globals->pop_break()) { doBreak = true;  break; }
						}
					}
					break;
				default:
					break;
				}
				if (doBreak || globals->pop_break()) { doBreak = true;  break; }
				++ITER;
			}
			globals->variables.erase("CHAR");
			globals->variables.erase("ITER");
		}
		else {
			id->eval(globals);
			StackVariable var = globals->pop_var();
			if (var.type != VariableType::STRING) {
				RuntimeError(VariableTypeToString(var.type) + " can't be used as an iterator");
			}
			std::string value = var.strValue;

			bool doBreak = false;
			int ITER = 0;
			for (auto& CHAR : value)
			{
				for (auto statment : statements)
				{
					globals->variables["CHAR"] = StackVariable(std::string(1, CHAR));
					globals->variables["ITER"] = ITER;
					statment->eval(globals);
					if (doBreak || globals->pop_break()) { doBreak = true;  break; }
				}
				if (doBreak || globals->pop_break()) { doBreak = true;  break; }

				++ITER;
			}
			globals->variables.erase("CHAR");
			globals->variables.erase("ITER");
		}
	}
};

class LOOP_DAY : public TreeNode
{
public:
	LOOP_DAY(std::vector<TreeNode*> statements) : statements(statements) {}
	virtual ~LOOP_DAY() override = default;
	std::vector<TreeNode*> statements;
public:
	virtual void print() override {
		std::cout << "LOOP DAY LINES : \n";
		for (auto statment : statements)
		{
			statment->print();
		}
		std::cout << "LOOPEND";

	}
	virtual void eval(RuntimeGlobals* globals) override
	{
		bool doBreak = false;
		int ITER = 0;
		for (auto& LINE : globals->DayLines)
		{
			for (auto statment : statements)
			{
				globals->variables["LINE"] = StackVariable(LINE);
				globals->variables["ITER"] = ITER;
				statment->eval(globals);
				if (doBreak || globals->pop_break()) { doBreak = true;  break; }
			}
			if (doBreak || globals->pop_break()) { doBreak = true;  break; }

			++ITER;
		}
		globals->variables.erase("LINE");
		globals->variables.erase("ITER");
	}
};

class BREAK : public TreeNode
{
public:
	BREAK() {}
	virtual ~BREAK() override = default;
public:
	virtual void print() override { std::cout << "BREAK"; }
	virtual void eval(RuntimeGlobals* globals) override
	{
		globals->push_break();
	}
};

class STRING : public TreeNode
{
public:
	STRING(std::string str) : str(str) {}
	virtual ~STRING() override = default;
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
	virtual ~INTEGER() override = default;
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
	virtual ~FLOAT() override = default;
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
	bool ScanListDeclaration(Token t, TreeNode** outNode);
	bool ScanID(Token t, TreeNode** outNode);
	bool ScanBreak(Token t, TreeNode** outNode);
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
	std::map<std::string, char> declaredLists;
};