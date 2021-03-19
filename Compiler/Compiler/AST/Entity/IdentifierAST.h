#pragma once
#include "../IExpressionAST.h"

class IdentifierAST : public IExpressionAST
{
public:
	explicit IdentifierAST(const std::string& name);
	const std::string& GetName() const;

	void Accept(IExpressionVisitor& visitor) const override;

private:
	std::string m_name;
};