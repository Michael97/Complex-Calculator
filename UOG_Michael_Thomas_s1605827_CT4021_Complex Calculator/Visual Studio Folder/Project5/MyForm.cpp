#include "MyForm.h"
#include "Token.h"
#include <cstdlib>
#include <iostream>
#include <stack>
#include <list>
#include <string>
#include <regex>
#include <stdexcept>

using namespace Calc;
using namespace System;
using namespace System::Windows::Forms;

std::list<Token> rpnList;

double evalRPN(std::list<Token> & rpnList) {
	std::stack<double> nums;
	for (std::list<Token>::iterator it = rpnList.begin(); it != rpnList.end(); ++it) {
		if (it->tokenType == Token::TokenType::Operator) {
			// pop two numbers off the stack               
			double a = nums.top();
			nums.pop();
			double b = nums.top();
			nums.pop();
			// evaluate and push the result back
			switch (it->symbol)
			{
			case '+': nums.push(a + b); break;
			case '-': nums.push(b - a); break;
			case '*': nums.push(b * a); break;
			case '/': nums.push(b / a); break;
			case '^': nums.push(pow(b, a)); break;
			default:;
			}
		}
		else if (it->tokenType == Token::TokenType::Number) {
			// push a number into the stack                 
			double n = it->value;
			std::cout << "n =" << n << std::endl;
			nums.push(n);
		}
		else
			std::cout << it->tokenType << " error" << std::endl;
	}

	std::cout << "Answer = " << nums.top() << std::endl;

	while (!rpnList.empty())
		rpnList.pop_back();

	return nums.top();
}

double StringToDouble(char **pos)
{
	//This function allows the use of floating point numbers
	double value;
	char *end = *pos;
	bool has_dot = false;
	while (isdigit(*end) || *end == '.') {
		if (*end == '\0')
			break;
		//checking for extra dots, if so throw error message to int main()
		if (*end == '.' && has_dot == true)
			throw std::runtime_error("Floating point number has too many dots");
		if (*end == '.')
			has_dot = true;
		++end;
	}
	value = strtod(&(**pos), &end);
	*pos = end - 1;
	return value;
}

std::list<Token> StringToTokenList(std::string str)
{
	Token t;
	std::list<Token> token_list;
	char *pos = const_cast<char*>(str.c_str());

	while (*pos) {
		t = Token();
		if (*pos == ' ') {
			pos++;
			continue;
		}

		if (isdigit(*pos)) {
			t.value = StringToDouble(&pos);
			t.tokenType = Token::TokenType::Number;
		}
		else if (*pos == '(')
			t.tokenType = Token::TokenType::Left_Paran;
		else if (*pos == ')')
			t.tokenType = Token::TokenType::Right_Paran;
		else if (*pos == '+')
		{
			t.tokenType = Token::TokenType::Operator;
			t.symbol = '+';
			t.precedence = 1;
		}
		else if (*pos == '-')
		{
			t.tokenType = Token::TokenType::Operator;
			t.symbol = '-';
			t.precedence = 1;
		}
		else if (*pos == '/')
		{
			t.tokenType = Token::TokenType::Operator;
			t.symbol = '/';
			t.precedence = 2;
		}
		else if (*pos == '*')
		{
			t.tokenType = Token::TokenType::Operator;
			t.symbol = '*';
			t.precedence = 2;
		}
		else if (*pos == '^')
		{
			t.tokenType = Token::TokenType::Operator;
			t.symbol = '^';
			t.precedence = 3;
		}
		else if (*pos == 'e') //exponential
		{
			t.tokenType = Token::TokenType::Number;
			t.value = 2.71828182845904523536;
		}
		else if (*pos == 'p') //pi
		{
			t.tokenType = Token::TokenType::Number;
			t.value = 3.14159265;
		}
		else
			t.tokenType = Token::TokenType::Nothing;
		token_list.push_back(t);
		if (*pos != '\0')
			pos++;
	}
	return token_list;
}

std::list<Token> shuntingYardAlgorithm(std::list<Token> token_list)
{
	std::stack<Token> shuntingYardStack;

	for (std::list<Token>::iterator it = token_list.begin(); it != token_list.end(); ++it) {

		if (it->tokenType == Token::TokenType::Operator)
		{
			if (!shuntingYardStack.empty())
			{
				if (shuntingYardStack.top().tokenType != Token::TokenType::Left_Paran && shuntingYardStack.top().tokenType != Token::TokenType::Right_Paran)
				{
					//while there is an operator on top of the stack with a greater precedence
					while (!shuntingYardStack.empty() && shuntingYardStack.top().precedence >= it->precedence)
					{
						//pop operators from the stack onto the output queue
						rpnList.push_back(shuntingYardStack.top());
						shuntingYardStack.pop();
						break;
					}
				}
			}
			shuntingYardStack.push(*it);
		}
		//if its a left backet push it onto the stack
		else if (it->tokenType == Token::TokenType::Left_Paran)
		{
			shuntingYardStack.push(*it);
		}
		//if its a right bracket
		else if (it->tokenType == Token::TokenType::Right_Paran)
		{
			while (it->tokenType != Token::TokenType::Left_Paran)
			{
				//pop operators from the stack onto the output queue
				if (!shuntingYardStack.top().tokenType != Token::TokenType::Left_Paran && !shuntingYardStack.top().tokenType != Token::TokenType::Right_Paran)
				{
					rpnList.push_back(shuntingYardStack.top());
					shuntingYardStack.pop();
					break;
				}
			}
			shuntingYardStack.pop();
		}

		//if its a number add it to the queue
		else if (it->tokenType == Token::TokenType::Number)
		{
			rpnList.push_back(*it);
		}

		else
		{
			std::cerr << "Error in expression" << std::endl;
			std::exit(1);
		}
	}

	//while there are tokens in the stack
	while (!shuntingYardStack.empty())
	{
		if (shuntingYardStack.top().tokenType == Token::TokenType::Number ||
			shuntingYardStack.top().tokenType == Token::TokenType::Operator)
		{
			rpnList.push_back(shuntingYardStack.top());
			shuntingYardStack.pop();
		}
		else if (shuntingYardStack.top().tokenType == Token::TokenType::Right_Paran ||
			shuntingYardStack.top().tokenType == Token::TokenType::Left_Paran)
		{
			std::cout << shuntingYardStack.top().tokenType << std::endl;
			shuntingYardStack.pop();

		}
	}
	return rpnList;
}

void debug(std::list<Token> list)
{
	for (std::list<Token>::iterator it = list.begin(); it != list.end(); ++it)
	{
		if (it->tokenType == Token::TokenType::Number)
			std::cout << "Integer = " << it->value << std::endl;
		else if (it->tokenType == Token::TokenType::Operator)
			std::cout << "Operator = " << it->symbol << std::endl;
		else if (it->tokenType == Token::TokenType::Left_Paran)
			std::cout << "Left Parenthesis" << std::endl;
		else if (it->tokenType == Token::TokenType::Right_Paran)
			std::cout << "Right Parenthesis" << std::endl;
		else if (it->tokenType == Token::TokenType::Nothing)
			std::cout << "Error in the parsing." << std::endl;
	}
}

double GetResultFromExpression(std::string infixExpression) {
	double result = 0;

	try {
		std::list<Token> token_list = StringToTokenList(infixExpression);
		debug(token_list);

		shuntingYardAlgorithm(token_list);

		std::cout << "Reverse Polish Notation" << std::endl;

		debug(rpnList);

		std::cout << "RPN Calculation Processing" << std::endl;

		result = evalRPN(rpnList);
	}
	//catches all thrown exception, whatever its types
	catch (std::exception &e) {
		std::cerr << "[Exception] " << e.what() << std::endl;
	}
	return result;
}

[STAThread]
int main(array< String^ > ^args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	Calc::MyForm form;
	Application::Run(%form);
	return 0;
}
