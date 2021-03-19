#include "LiteralConstantAST.h"

LiteralConstantAST::LiteralConstantAST(const LiteralConstantAST::Value& value)
	: m_value(value)
{
}

const LiteralConstantAST::Value& LiteralConstantAST::GetValue() const
{
	return m_value;
}

void LiteralConstantAST::Accept(IExpressionVisitor& visitor) const
{
	visitor.Visit(*this);
}