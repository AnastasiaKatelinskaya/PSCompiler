#include "FunctionCallStatementAST.h"

FunctionCallStatementAST::FunctionCallStatementAST(std::unique_ptr<FunctionCallExpressionAST>&& call)
	: m_call(std::move(call))
{
}

const IExpressionAST& FunctionCallStatementAST::GetCall() const
{
	return *m_call;
}

const FunctionCallExpressionAST& FunctionCallStatementAST::GetCallAsDerived() const
{
	return *m_call;
}

void FunctionCallStatementAST::Accept(IStatementVisitor& visitor) const
{
	visitor.Visit(*this);
}