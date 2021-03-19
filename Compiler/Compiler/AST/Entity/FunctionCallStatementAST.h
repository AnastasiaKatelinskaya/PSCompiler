#pragma once
#include "../IExpressionAST.h"
#include "../IStatementAST.h"
#include "FunctionCallExpressionAST.h"

// Function call statement with ignoring returning value of a function
class FunctionCallStatementAST : public IStatementAST
{
public:
	explicit FunctionCallStatementAST(std::unique_ptr<FunctionCallExpressionAST>&& call);
	const IExpressionAST& GetCall() const;
	const FunctionCallExpressionAST& GetCallAsDerived() const;

	void Accept(IStatementVisitor& visitor) const override;

private:
	std::unique_ptr<FunctionCallExpressionAST> m_call;
};