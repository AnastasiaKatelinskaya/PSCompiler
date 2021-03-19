#pragma once
#include "../IExpressionAST.h"

#include <variant>

class LiteralConstantAST : public IExpressionAST
{
public:
	using Value = std::variant<
		int,
		double,
		bool,
		std::string,
		std::vector<std::shared_ptr<IExpressionAST>>>;

	explicit LiteralConstantAST(const Value& value);
	const Value& GetValue() const;
	void Accept(IExpressionVisitor& visitor) const override;

private:
	Value m_value;
};