#pragma once
#include "../IExpressionAST.h"

#include <stdexcept>

class ArrayElementAccessAST : public IExpressionAST
{
public:
	explicit ArrayElementAccessAST(const std::string& name, std::unique_ptr<IExpressionAST>&& index);

	const std::string& GetName() const;

	size_t GetIndexCount() const;
	const IExpressionAST& GetIndex(size_t index = 0) const;
	void AddIndex(std::unique_ptr<IExpressionAST>&& index);

	void Accept(IExpressionVisitor& visitor) const override;

private:
	std::string m_name;
	std::vector<std::unique_ptr<IExpressionAST>> m_indices;
};