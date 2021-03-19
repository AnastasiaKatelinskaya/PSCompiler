#pragma once
#include "../IExpressionAST.h"
#include "../IStatementAST.h"
#include "IdentifierAST.h"

class AssignStatementAST : public IStatementAST
{
public:
	explicit AssignStatementAST(
		std::unique_ptr<IdentifierAST>&& identifier,
		std::unique_ptr<IExpressionAST>&& expr);

	const IdentifierAST& GetIdentifier() const;
	const IExpressionAST& GetExpr() const;

	void Accept(IStatementVisitor& visitor) const override;

private:
	std::unique_ptr<IdentifierAST> m_identifier;
	std::unique_ptr<IExpressionAST> m_expr;
};