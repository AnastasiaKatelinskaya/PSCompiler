#include "CompositeStatementAST.h"

void CompositeStatementAST::AddStatement(std::unique_ptr<IStatementAST>&& stmt)
{
	m_statements.push_back(std::move(stmt));
}

const IStatementAST& CompositeStatementAST::GetStatement(size_t index) const
{
	if (index >= m_statements.size())
	{
		throw std::out_of_range("index must be less that statements count");
	}
	return *m_statements[index];
}

size_t CompositeStatementAST::GetCount() const
{
	return m_statements.size();
}

void CompositeStatementAST::Accept(IStatementVisitor& visitor) const
{
	visitor.Visit(*this);
}