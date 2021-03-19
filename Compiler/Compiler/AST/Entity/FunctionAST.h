#pragma once
#include "../ExpressionType.h"
#include "../IStatementAST.h"
#include "IdentifierAST.h"

class FunctionAST
{
public:
	using Param = std::pair<std::string, ExpressionType>;

	explicit FunctionAST(
		ExpressionType returnType,
		std::unique_ptr<IdentifierAST>&& identifier,
		std::vector<Param>&& params,
		std::unique_ptr<IStatementAST>&& statement);

	ExpressionType GetReturnType() const;
	const IdentifierAST& GetIdentifier() const;
	const std::vector<Param>& GetParams() const;
	const IStatementAST& GetStatement() const;

private:
	ExpressionType m_returnType;
	std::vector<Param> m_params;
	std::unique_ptr<IdentifierAST> m_identifier;
	std::unique_ptr<IStatementAST> m_statement;
};