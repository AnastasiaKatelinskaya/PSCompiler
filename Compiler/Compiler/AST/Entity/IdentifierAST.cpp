#include "IdentifierAST.h"

IdentifierAST::IdentifierAST(const std::string& name)
	: m_name(name)
{
}

const std::string& IdentifierAST::GetName() const
{
	return m_name;
}

void IdentifierAST::Accept(IExpressionVisitor& visitor) const
{
	visitor.Visit(*this);
}