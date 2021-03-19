#pragma once
#include "../ExpressionType.h"
#include "../IExpressionAST.h"
#include "../IStatementAST.h"
#include "IdentifierAST.h"

class VariableDeclarationAST : public IStatementAST
{
public:
	explicit VariableDeclarationAST(std::unique_ptr<IdentifierAST>&& identifier, ExpressionType type);

	void SetExpression(std::unique_ptr<IExpressionAST>&& expr);
	const IExpressionAST& GetExpression() const;

	const IdentifierAST& GetIdentifier() const;
	ExpressionType GetType() const;

	void Accept(IStatementVisitor& visitor) const override;

private:
	std::unique_ptr<IdentifierAST> m_identifier;
	ExpressionType m_type;
	std::unique_ptr<IExpressionAST> m_expr;
};