#pragma once
#include "../IExpressionAST.h"
#include "../IStatementAST.h"

#include <stdexcept>

class BuiltinCallStatementAST : public IStatementAST
{
public:
	enum class Builtin
	{
		Print,
		Read
	};

	explicit BuiltinCallStatementAST(Builtin builtin);

	Builtin GetBuiltin() const;
	size_t GetParamsCount() const;
	const IExpressionAST& GetExpression(size_t index) const;
	void AddExpression(std::unique_ptr<IExpressionAST>&& expression);

	void Accept(IStatementVisitor& visitor) const override;

private:
	Builtin m_builtin;
	std::vector<std::unique_ptr<IExpressionAST>> m_params;
};