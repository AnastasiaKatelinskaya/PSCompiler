#include "WhileStatementAST.h"

WhileStatementAST::WhileStatementAST(std::unique_ptr<IExpressionAST>&& expr, std::unique_ptr<IStatementAST>&& stmt)
	: m_expr(std::move(expr))
	, m_stmt(std::move(stmt))
{
}

const IExpressionAST& WhileStatementAST::GetExpr() const
{
	return *m_expr;
}

const IStatementAST& WhileStatementAST::GetStatement() const
{
	return *m_stmt;
}

void WhileStatementAST::Accept(IStatementVisitor& visitor) const
{
	visitor.Visit(*this);
}