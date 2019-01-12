#pragma once

#include <list>
#include <string>

struct Token {
	enum TokenType {
		Number, Operator, Left_Paran, Right_Paran, Nothing
	} type;

	//For all tokens
	TokenType tokenType = TokenType::Nothing;
	//If it's a number
	double value = 0.0;
	//If it's an operator
	char symbol = ' ';
	int precedence = 0;
};

// prototypes
double GetResultFromExpression(std::string infixExpression);