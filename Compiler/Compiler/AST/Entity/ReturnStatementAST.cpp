#include "ReturnStatementAST.h"

ReturnStatementAST::ReturnStatementAST(std::unique_ptr<IExpressionAST>&& expression)
	: m_expression(std::move(expression))
{
}

const IExpressionAST* ReturnStatementAST::GetExpression() const
{
	return m_expression.get();
}

void ReturnStatementAST::Accept(IStatementVisitor& visitor) const
{
	visitor.Visit(*this);
}