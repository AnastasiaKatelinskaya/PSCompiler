#pragma once
#include "../IExpressionAST.h"

class BinaryExpressionAST : public IExpressionAST
{
public:
	enum class Operator
	{
		Or,
		And,
		Equals,
		NotEquals,
		Less,
		LessOrEquals,
		More,
		MoreOrEquals,
		Plus,
		Minus,
		Multiplication,
		Division,
		Modulo,
	};

	explicit BinaryExpressionAST(std::unique_ptr<IExpressionAST>&& left, std::unique_ptr<IExpressionAST>&& right, Operator op);

	const IExpressionAST& GetLeft() const;
	const IExpressionAST& GetRight() const;
	Operator GetOperator() const;

	void Accept(IExpressionVisitor& visitor) const override;

private:
	std::unique_ptr<IExpressionAST> m_left;
	std::unique_ptr<IExpressionAST> m_right;
	Operator m_op;
};