#pragma once
#include "Visitor/IExpressionVisitor.h"

#include <memory>
#include <string>
#include <vector>

class IExpressionAST
{
public:
	using Ptr = std::unique_ptr<IExpressionAST>;
	virtual ~IExpressionAST() = default;
	virtual void Accept(IExpressionVisitor& visitor) const = 0;
};