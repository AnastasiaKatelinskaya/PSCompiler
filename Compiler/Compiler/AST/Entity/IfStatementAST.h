#pragma once
#include "../IExpressionAST.h"
#include "../IStatementAST.h"

class IfStatementAST : public IStatementAST
{
public:
	explicit IfStatementAST(
		std::unique_ptr<IExpressionAST>&& expr,
		std::unique_ptr<IStatementAST>&& then,
		std::unique_ptr<IStatementAST>&& elif = nullptr);

	void SetElseClause(std::unique_ptr<IStatementAST>&& elif);
	const IExpressionAST& GetExpr() const;
	const IStatementAST& GetThenStmt() const;
	const IStatementAST* GetElseStmt() const;

	void Accept(IStatementVisitor& visitor) const override;

private:
	std::unique_ptr<IExpressionAST> m_expr;
	std::unique_ptr<IStatementAST> m_then;
	std::unique_ptr<IStatementAST> m_elif;
};