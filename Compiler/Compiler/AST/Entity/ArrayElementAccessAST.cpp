#include "ArrayElementAccessAST.h"

ArrayElementAccessAST::ArrayElementAccessAST(const std::string& name, std::unique_ptr<IExpressionAST>&& index)
	: m_name(name)
	, m_indices()
{
	m_indices.push_back(std::move(index));
}

const std::string& ArrayElementAccessAST::GetName() const
{
	return m_name;
}

size_t ArrayElementAccessAST::GetIndexCount() const
{
	return m_indices.size();
}

const IExpressionAST& ArrayElementAccessAST::GetIndex(size_t index /* = 0 */) const
{
	if (index >= m_indices.size())
	{
		throw std::out_of_range("index must be less than indices count in array element access");
	}
	return *m_indices[index];
}

void ArrayElementAccessAST::AddIndex(std::unique_ptr<IExpressionAST>&& index)
{
	m_indices.push_back(std::move(index));
}

void ArrayElementAccessAST::Accept(IExpressionVisitor& visitor) const
{
	visitor.Visit(*this);
}