#pragma once
#include "../IExpressionAST.h"
#include "../IStatementAST.h"

class WhileStatementAST : public IStatementAST
{
public:
	explicit WhileStatementAST(std::unique_ptr<IExpressionAST>&& expr, std::unique_ptr<IStatementAST>&& stmt);

	const IExpressionAST& GetExpr() const;
	const IStatementAST& GetStatement() const;

	void Accept(IStatementVisitor& visitor) const override;

private:
	std::unique_ptr<IExpressionAST> m_expr;
	std::unique_ptr<IStatementAST> m_stmt;
};