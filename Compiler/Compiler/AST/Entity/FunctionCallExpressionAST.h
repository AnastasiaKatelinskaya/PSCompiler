#pragma once
#include "../IExpressionAST.h"

#include <stdexcept>

class FunctionCallExpressionAST : public IExpressionAST
{
public:
	explicit FunctionCallExpressionAST(const std::string& name, std::vector<std::unique_ptr<IExpressionAST>>&& params);

	const std::string& GetName() const;
	size_t GetParamsCount() const;
	const IExpressionAST& GetParam(size_t index) const;

	void Accept(IExpressionVisitor& visitor) const override;

private:
	std::string m_name;
	std::vector<std::unique_ptr<IExpressionAST>> m_params;
};