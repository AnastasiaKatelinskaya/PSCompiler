#pragma once
#include "../IStatementAST.h"

#include <stdexcept>

class CompositeStatementAST : public IStatementAST
{
public:
	void AddStatement(std::unique_ptr<IStatementAST>&& stmt);
	const IStatementAST& GetStatement(size_t index) const;
	size_t GetCount() const;

	void Accept(IStatementVisitor& visitor) const override;

private:
	std::vector<std::unique_ptr<IStatementAST>> m_statements;
};