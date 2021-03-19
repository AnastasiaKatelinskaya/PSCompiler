#pragma once

class LiteralConstantAST;
class IdentifierAST;
class BinaryExpressionAST;
class UnaryAST;
class FunctionCallExpressionAST;
class ArrayElementAccessAST;

class IExpressionVisitor
{
public:
	virtual ~IExpressionVisitor() = default;
	virtual void Visit(const BinaryExpressionAST& node) = 0;
	virtual void Visit(const LiteralConstantAST& node) = 0;
	virtual void Visit(const UnaryAST& node) = 0;
	virtual void Visit(const IdentifierAST& node) = 0;
	virtual void Visit(const FunctionCallExpressionAST& node) = 0;
	virtual void Visit(const ArrayElementAccessAST& node) = 0;
};