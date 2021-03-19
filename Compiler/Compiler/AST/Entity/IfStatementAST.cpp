#include "IfStatementAST.h"

IfStatementAST::IfStatementAST(
	std::unique_ptr<IExpressionAST>&& expr,
	std::unique_ptr<IStatementAST>&& then,
	std::unique_ptr<IStatementAST>&& elif)
	: m_expr(std::move(expr))
	, m_then(std::move(then))
	, m_elif(std::move(elif))
{
}

void IfStatementAST::SetElseClause(std::unique_ptr<IStatementAST>&& elif)
{
	m_elif = std::move(elif);
}

const IExpressionAST& IfStatementAST::GetExpr() const
{
	return *m_expr;
}

const IStatementAST& IfStatementAST::GetThenStmt() const
{
	return *m_then;
}

const IStatementAST* IfStatementAST::GetElseStmt() const
{
	return m_elif ? m_elif.get() : nullptr;
}

void IfStatementAST::Accept(IStatementVisitor& visitor) const
{
	visitor.Visit(*this);
}