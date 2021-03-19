#include "FunctionAST.h"

FunctionAST::FunctionAST(
	ExpressionType returnType,
	std::unique_ptr<IdentifierAST>&& identifier,
	std::vector<Param>&& params,
	std::unique_ptr<IStatementAST>&& statement)
	: m_returnType(returnType)
	, m_identifier(std::move(identifier))
	, m_params(std::move(params))
	, m_statement(std::move(statement))
{
}

ExpressionType FunctionAST::GetReturnType() const
{
	return m_returnType;
}

const IdentifierAST& FunctionAST::GetIdentifier() const
{
	return *m_identifier;
}

const std::vector<FunctionAST::Param>& FunctionAST::GetParams() const
{
	return m_params;
}

const IStatementAST& FunctionAST::GetStatement() const
{
	return *m_statement;
}