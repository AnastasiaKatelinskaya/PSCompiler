#pragma once
#include "Visitor/IStatementVisitor.h"

#include <memory>
#include <string>
#include <vector>

class IStatementAST
{
public:
	using Ptr = std::unique_ptr<IStatementAST>;
	virtual ~IStatementAST() = default;
	virtual void Accept(IStatementVisitor& visitor) const = 0;
};