#pragma once
#include "../IExpressionAST.h"
#include "../IStatementAST.h"

class ReturnStatementAST : public IStatementAST
{
public:
	explicit ReturnStatementAST(std::unique_ptr<IExpressionAST>&& expression);

	const IExpressionAST* GetExpression() const;
	void Accept(IStatementVisitor& visitor) const override;

private:
	std::unique_ptr<IExpressionAST> m_expression;
};