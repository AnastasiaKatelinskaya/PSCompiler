#include "FunctionCallExpressionAST.h"

FunctionCallExpressionAST::FunctionCallExpressionAST(const std::string& name, std::vector<std::unique_ptr<IExpressionAST>>&& params)
	: m_name(name)
	, m_params(std::move(params))
{
}

const std::string& FunctionCallExpressionAST::GetName() const
{
	return m_name;
}

size_t FunctionCallExpressionAST::GetParamsCount() const
{
	return m_params.size();
}

const IExpressionAST& FunctionCallExpressionAST::GetParam(size_t index) const
{
	if (index < m_params.size())
	{
		return *m_params[index];
	}
	throw std::runtime_error("index must be less than function call's params count");
}

void FunctionCallExpressionAST::Accept(IExpressionVisitor& visitor) const
{
	visitor.Visit(*this);
}