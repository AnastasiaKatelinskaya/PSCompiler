#include "BuiltinCallStatementAST.h"

BuiltinCallStatementAST::BuiltinCallStatementAST(Builtin builtin)
	: m_builtin(builtin)
{
}

BuiltinCallStatementAST::Builtin BuiltinCallStatementAST::GetBuiltin() const
{
	return m_builtin;
}

size_t BuiltinCallStatementAST::GetParamsCount() const
{
	return m_params.size();
}

const IExpressionAST& BuiltinCallStatementAST::GetExpression(size_t index) const
{
	if (index >= m_params.size())
	{
		throw std::out_of_range("index must be less than expressions count");
	}
	return *m_params[index];
}

void BuiltinCallStatementAST::AddExpression(std::unique_ptr<IExpressionAST>&& expression)
{
	m_params.push_back(std::move(expression));
}

void BuiltinCallStatementAST::Accept(IStatementVisitor& visitor) const
{
	visitor.Visit(*this);
}