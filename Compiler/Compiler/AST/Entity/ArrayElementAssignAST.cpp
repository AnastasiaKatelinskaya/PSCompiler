#include "ArrayElementAssignAST.h"

ArrayElementAssignAST::ArrayElementAssignAST(std::unique_ptr<ArrayElementAccessAST>&& access, std::unique_ptr<IExpressionAST>&& expression)
	: m_access(std::move(access))
	, m_expression(std::move(expression))
{
}

size_t ArrayElementAssignAST::GetIndexCount() const
{
	return m_access->GetIndexCount();
}

const std::string& ArrayElementAssignAST::GetName() const
{
	return m_access->GetName();
}

const IExpressionAST& ArrayElementAssignAST::GetIndex(size_t index /* = 0 */) const
{
	return m_access->GetIndex(index);
}

const IExpressionAST& ArrayElementAssignAST::GetExpression() const
{
	return *m_expression;
}

void ArrayElementAssignAST::Accept(IStatementVisitor& visitor) const
{
	visitor.Visit(*this);
}