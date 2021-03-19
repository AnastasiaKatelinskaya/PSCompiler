#include "AssignStatementAST.h"

AssignStatementAST::AssignStatementAST(
	std::unique_ptr<IdentifierAST>&& identifier,
	std::unique_ptr<IExpressionAST>&& expr)
	: m_identifier(std::move(identifier))
	, m_expr(std::move(expr))
{
}

const IdentifierAST& AssignStatementAST::GetIdentifier() const
{
	return *m_identifier;
}

const IExpressionAST& AssignStatementAST::GetExpr() const
{
	return *m_expr;
}

void AssignStatementAST::Accept(IStatementVisitor& visitor) const
{
	visitor.Visit(*this);
}