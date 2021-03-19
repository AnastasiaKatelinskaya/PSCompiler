#pragma once
#include "../IExpressionAST.h"

class UnaryAST : public IExpressionAST
{
public:
	enum class Operator
	{
		Plus,
		Minus,
		Negation
	};

	explicit UnaryAST(std::unique_ptr<IExpressionAST>&& expr, Operator op);

	const IExpressionAST& GetExpr() const;
	Operator GetOperator() const;

	void Accept(IExpressionVisitor& visitor) const override;

private:
	std::unique_ptr<IExpressionAST> m_expr;
	Operator m_op;
};