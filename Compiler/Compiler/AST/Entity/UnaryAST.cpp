#include "UnaryAST.h"

UnaryAST::UnaryAST(std::unique_ptr<IExpressionAST>&& expr, UnaryAST::Operator op)
	: m_expr(std::move(expr))
	, m_op(op)
{
}

const IExpressionAST& UnaryAST::GetExpr() const
{
	return *m_expr;
}

UnaryAST::Operator UnaryAST::GetOperator() const
{
	return m_op;
}

void UnaryAST::Accept(IExpressionVisitor& visitor) const
{
	visitor.Visit(*this);
}