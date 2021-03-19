#include "BinaryExpressionAST.h"

BinaryExpressionAST::BinaryExpressionAST(std::unique_ptr<IExpressionAST>&& left, std::unique_ptr<IExpressionAST>&& right, BinaryExpressionAST::Operator op)
	: m_left(std::move(left))
	, m_right(std::move(right))
	, m_op(op)
{
}

const IExpressionAST& BinaryExpressionAST::GetLeft() const
{
	return *m_left;
}

const IExpressionAST& BinaryExpressionAST::GetRight() const
{
	return *m_right;
}

BinaryExpressionAST::Operator BinaryExpressionAST::GetOperator() const
{
	return m_op;
}

void BinaryExpressionAST::Accept(IExpressionVisitor& visitor) const
{
	visitor.Visit(*this);
}