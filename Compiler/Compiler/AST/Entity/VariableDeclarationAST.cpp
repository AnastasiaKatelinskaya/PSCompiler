#include "VariableDeclarationAST.h"

VariableDeclarationAST::VariableDeclarationAST(std::unique_ptr<IdentifierAST>&& identifier, ExpressionType type)
	: m_identifier(std::move(identifier))
	, m_type(type)
	, m_expr(nullptr)
{
}

void VariableDeclarationAST::SetExpression(std::unique_ptr<IExpressionAST>&& expr)
{
	m_expr = std::move(expr);
}

const IExpressionAST& VariableDeclarationAST::GetExpression() const
{
	return *m_expr;
}

const IdentifierAST& VariableDeclarationAST::GetIdentifier() const
{
	return *m_identifier;
}

ExpressionType VariableDeclarationAST::GetType() const
{
	return m_type;
}

void VariableDeclarationAST::Accept(IStatementVisitor& visitor) const
{
	visitor.Visit(*this);
}