#pragma once
#include "ArrayElementAccessAST.h"
#include "../IExpressionAST.h"
#include "../IStatementAST.h"

class ArrayElementAssignAST : public IStatementAST
{
public:
	explicit ArrayElementAssignAST(std::unique_ptr<ArrayElementAccessAST>&& access, std::unique_ptr<IExpressionAST>&& expression);

	size_t GetIndexCount() const;
	const std::string& GetName() const;
	const IExpressionAST& GetIndex(size_t index = 0) const;
	const IExpressionAST& GetExpression() const;

	void Accept(IStatementVisitor& visitor) const override;

private:
	std::unique_ptr<ArrayElementAccessAST> m_access;
	std::unique_ptr<IExpressionAST> m_expression;
};