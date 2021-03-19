#include "AST.h"

std::string ToString(BinaryExpressionAST::Operator operation)
{
	switch (operation)
	{
	case BinaryExpressionAST::Operator::Or:
		return "||";
	case BinaryExpressionAST::Operator::And:
		return "&&";
	case BinaryExpressionAST::Operator::Equals:
		return "==";
	case BinaryExpressionAST::Operator::NotEquals:
		return "!=";
	case BinaryExpressionAST::Operator::Less:
		return "<";
	case BinaryExpressionAST::Operator::LessOrEquals:
		return "<=";
	case BinaryExpressionAST::Operator::More:
		return ">";
	case BinaryExpressionAST::Operator::MoreOrEquals:
		return ">=";
	case BinaryExpressionAST::Operator::Plus:
		return "+";
	case BinaryExpressionAST::Operator::Minus:
		return "-";
	case BinaryExpressionAST::Operator::Multiplication:
		return "*";
	case BinaryExpressionAST::Operator::Division:
		return "/";
	case BinaryExpressionAST::Operator::Modulo:
		return "%";
	}
	throw std::logic_error("ToString: can't cast undefined binary operator to string");
}

std::string ToString(UnaryAST::Operator operation)
{
	switch (operation)
	{
	case UnaryAST::Operator::Plus:
		return "+";
	case UnaryAST::Operator::Minus:
		return "-";
	case UnaryAST::Operator::Negation:
		return "!";
	}
	throw std::logic_error("ToString: can't cast undefined unary operator to string");
}

std::string ToString(BuiltinCallStatementAST::Builtin builtIn)
{
	switch (builtIn)
	{
	case BuiltinCallStatementAST::Builtin::Print:
		return "print";
	case BuiltinCallStatementAST::Builtin::Read:
		return "read";
	default:
		throw std::exception("Non-exhaustive switch");
	}
}
