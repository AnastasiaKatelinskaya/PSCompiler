#pragma once

#include "../Visitor/IStatementVisitor.h"
#include "../Visitor/IExpressionVisitor.h"

#include "../AST.h"
#include "../Entity/ProgramAST.h"
#include "../Entity/ArrayElementAccessAST.h"
#include "../Entity/ArrayElementAssignAST.h"
#include "../Entity/AssignStatementAST.h"
#include "../Entity/BinaryExpressionAST.h"
#include "../Entity/BuiltinCallStatementAST.h"
#include "../Entity/CompositeStatementAST.h"
#include "../Entity/FunctionAST.h"
#include "../Entity/FunctionCallExpressionAST.h"
#include "../Entity/FunctionCallStatementAST.h"
#include "../Entity/IdentifierAST.h"
#include "../Entity/IfStatementAST.h"
#include "../Entity/LiteralConstantAST.h"
#include "../Entity/ProgramAST.h"
#include "../Entity/ReturnStatementAST.h"
#include "../Entity/UnaryAST.h"
#include "../Entity/VariableDeclarationAST.h"
#include "../Entity/WhileStatementAST.h"

#include "../ExpressionType.h"

#include <algorithm>
#include <iostream>
#include <stack>
#include <set>

std::stack<std::map<std::string, ExpressionType>> scopes;
std::stack<ExpressionType> expressionTypes;
std::string currFn;

ExpressionType GetExpressionType(const IExpressionAST& expr, IExpressionVisitor& visitor)
{
	expr.Accept(visitor);
	const auto type = expressionTypes.top();
	expressionTypes.pop();
	return type;
}

void CheckType(ExpressionType testType, std::vector<ExpressionType> expectedTypes)
{
	if (const auto it = std::find(expectedTypes.cbegin(), expectedTypes.cend(), testType); it != expectedTypes.cend())
		return;
	bool isConvertiable = false;
	for (const auto& type : expectedTypes)
		isConvertiable |= IsConvertible(testType, type);
	if (!isConvertiable)
		throw std::exception("[ERROR] Type"); // TODO: at ?:?
	std::cerr << "[WARNING] Type cast" << std::endl;
}

void CheckType(std::vector<ExpressionType> testTypes, std::vector<ExpressionType> expectedTypes)
{
	if (testTypes.size() != expectedTypes.size())
		throw std::exception("[ERROR] Invalid arguments count"); // TODO: at ?:?
	for (size_t i = 0; i < testTypes.size(); ++i)
		CheckType(testTypes[i], { expectedTypes[i] });
}

std::optional<ExpressionType> GetPreferredTypeWithWarn(const ExpressionType& left, const ExpressionType& right)
{
	if (left == right)
		return left;

	const auto preferredType = GetPreferredType(left, right);
	if (!preferredType)
		throw std::exception("[ERROR] No preffered type in binary expression"); // TODO: at ?:? (and not in binary expressions only)
	std::cerr << "[WARNING] Type cast" << std::endl; // TODO: at ?:?
	return preferredType;
}

ExpressionType GetVarType(std::string name)
{
	auto scope = scopes.top();
	if (!scope.count(name))
		throw std::exception(std::string("[ERROR] Unknown identifier '" + name + "'").c_str()); // TODO: at ?:?
	return scope[name];
}

std::pair<ExpressionType, std::vector<ExpressionType>> GetFnType(std::map<std::string, std::pair<ExpressionType, std::vector<ExpressionType>>> fnTypes, std::string name)
{
	if (!fnTypes.count(name))
		throw std::exception(std::string("[ERROR] Unknown function '" + name + "'").c_str()); // TODO: at ?:?
	return fnTypes[name];
}

template <class T>
void WithNewScope(T&& fn)
{
	const auto scope = scopes.top();
	scopes.push(scope);
	fn();
	scopes.pop();
}

void WithExtendedScope(std::string id, ExpressionType type)
{
	auto& scope = scopes.top();
	if (scope.count(id))
	{
		std::cerr << "[WARNING]" << " Variable " << id << " shadow" << std::endl;
	}
	scope[id] = type;
}

template <class T>
void WithNewFunctionScope(std::vector<std::pair<std::string, ExpressionType>> bla, T&& fn)
{
	std::map<std::string, ExpressionType> scope;
	for (const auto& [id, type] : bla)
	{
		scope[id] = type;
	}

	scopes.push(scope);
	fn();
	scopes.pop();
}

std::vector<ExpressionType> FunctionParamsToTypes(std::vector<FunctionAST::Param> args)
{
	std::vector<ExpressionType> argTypes;
	for (const auto& [argName, argType] : args)
	{
		argTypes.push_back(argType);
	}
	return argTypes;
}

template<class> inline constexpr bool always_false_v = false;

class TypeCheckingExpressionVisitor : public IExpressionVisitor
{
public:
	TypeCheckingExpressionVisitor(std::map<std::string, std::pair<ExpressionType, std::vector<ExpressionType>>> fnTypes)
		:m_fnTypes(fnTypes)
	{}

	void Visit(const BinaryExpressionAST& node) override
	{
		const auto& oper = node.GetOperator();

		const auto& left = node.GetLeft();
		const auto& right = node.GetRight();
		
		const auto leftType = GetExpressionType(left, *this);
		const auto rightType = GetExpressionType(right, *this);

		using Operator = BinaryExpressionAST::Operator;
		switch (oper)
		{
			case Operator::Or:
			case Operator::And:
				CheckType({ leftType, rightType }, { { ExpressionType::Value::Bool }, { ExpressionType::Value::Bool } });
				[[fallthrough]];
			case Operator::Equals:
			case Operator::NotEquals:
			case Operator::Less:
			case Operator::LessOrEquals:
			case Operator::More:
			case Operator::MoreOrEquals:
				if (const auto preferredType = GetPreferredTypeWithWarn(leftType, rightType); preferredType)
					expressionTypes.push({ ExpressionType::Value::Bool });
				break;
			case Operator::Plus:
			case Operator::Minus:
			case Operator::Multiplication:
			case Operator::Division:
				if (const auto preferredType = GetPreferredTypeWithWarn(leftType, rightType); preferredType)
					expressionTypes.push(*preferredType);
				break;
			case Operator::Modulo:
				CheckType({ leftType, rightType }, { { ExpressionType::Value::Int }, { ExpressionType::Value::Int } });
				expressionTypes.push({ ExpressionType::Value::Int });
				break;
			default:
				throw std::exception("Unknown switch (operator)");
		}
	}

	void Visit(const LiteralConstantAST& node) override
	{
		const auto& value = node.GetValue();
		std::visit([this](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, int>)
				expressionTypes.push({ ExpressionType::Value::Int });
			else if constexpr (std::is_same_v<T, double>)
				expressionTypes.push({ ExpressionType::Value::Float });
			else if constexpr (std::is_same_v<T, bool>)
				expressionTypes.push({ ExpressionType::Value::Bool });
			else if constexpr (std::is_same_v<T, std::string>)
				expressionTypes.push({ ExpressionType::Value::String });
			else if constexpr (std::is_same_v<T, std::vector<std::shared_ptr<IExpressionAST>>>) {
				if (arg.empty())
					throw std::exception("[ERROR] Empty arrays are not allowed"); // TODO: at ?:?
				const auto type = GetExpressionType(*(arg.front()), *this);
				std::vector<ExpressionType> argTypes;
				for (const auto& val : arg) {
					const auto argType = GetExpressionType(*val, *this);
					argTypes.push_back(argType);
				}
				if (const auto isTheSameType = std::all_of(argTypes.cbegin(), argTypes.cend(), [argTypes](ExpressionType type) {
						return type == argTypes.front();
					}); !isTheSameType)
						throw std::exception("[ERROR] Array with not the same types"); // TODO: at ?:?
				expressionTypes.push({ type.value, type.nesting + 1 });
			}
			else
				static_assert(always_false_v<T>, "Non-exhaustive visitor!");
		}, value);
	}

	void Visit(const UnaryAST& node) override
	{
		const auto& oper = node.GetOperator();
		const auto& expr = node.GetExpr();
		const auto exprType = GetExpressionType(expr, *this);
		switch (oper)
		{
			case UnaryAST::Operator::Minus:
			case UnaryAST::Operator::Plus:
				CheckType(exprType, { { ExpressionType::Value::Int }, { ExpressionType::Value::Float } });
				break;
			case UnaryAST::Operator::Negation:
				CheckType(exprType, { { ExpressionType::Value::Bool } });
				break;
			default:
				throw std::exception("Unknown switch (operator)");
		}
		expressionTypes.push(exprType);
	}

	void Visit(const IdentifierAST& node) override
	{
		const auto name = node.GetName();
		const auto type = GetVarType(name);
		expressionTypes.push(type);
	}

	void Visit(const FunctionCallExpressionAST& node) override
	{
		const auto& fnName = node.GetName();
		const auto [retType, argTypes] = GetFnType(m_fnTypes, fnName);
		
		std::vector<ExpressionType> paramTypes;
		for (size_t i = 0; i < node.GetParamsCount(); ++i)
		{
			const auto& expr = node.GetParam(i);
			const auto exprType = GetExpressionType(expr, *this);
			paramTypes.push_back(exprType);
		}
		CheckType(paramTypes, argTypes);
		expressionTypes.push(retType);
	}

	void Visit(const ArrayElementAccessAST& node) override
	{
		for (size_t i = 0; i < node.GetIndexCount(); ++i)
		{
			const auto& index = node.GetIndex(i);
			const auto indexType = GetExpressionType(index, *this);
			CheckType(indexType, { { ExpressionType::Value::Int } });
		}
		const auto [primitiveType, nesting] = GetVarType(node.GetName());
		expressionTypes.push({ primitiveType, nesting - node.GetIndexCount() });
	}

private:
	std::map<std::string, std::pair<ExpressionType, std::vector<ExpressionType>>> m_fnTypes;
};

class TypeCheckingStatementVisitor : public IStatementVisitor
{
public:
	TypeCheckingStatementVisitor(std::map<std::string, std::pair<ExpressionType, std::vector<ExpressionType>>> fnTypes)
		:m_fnTypes(fnTypes)
		,m_visitor(fnTypes)
	{}

	void Visit(const VariableDeclarationAST& node) override
	{
		const auto nodeType = node.GetType();
		const auto& id = node.GetIdentifier();

		const auto& expr = node.GetExpression();
		const auto exprType = GetExpressionType(expr, m_visitor);
		CheckType(exprType, { nodeType });

		WithExtendedScope(id.GetName(), nodeType);
	}

	void Visit(const AssignStatementAST& node) override
	{
		const auto& id = node.GetIdentifier();
		const auto& expr = node.GetExpr();
		const auto exprType = GetExpressionType(expr, m_visitor);
		CheckType(scopes.top()[id.GetName()], { exprType });
	}

	void Visit(const ArrayElementAssignAST& node) override
	{
		const auto varType = GetVarType(node.GetName());

		for (size_t i = 0; i < node.GetIndexCount(); ++i)
		{
			const auto& index = node.GetIndex(i);
			const auto indexType = GetExpressionType(index, m_visitor);
			CheckType(indexType, { { ExpressionType::Value::Int } });
		}

		const auto& expr = node.GetExpression();
		const auto exprType = GetExpressionType(expr, m_visitor);
		CheckType(exprType, { {varType.value, varType.nesting - node.GetIndexCount() } });
	}

	void Visit(const ReturnStatementAST& node) override
	{
		if (const auto& expr = node.GetExpression(); expr)
		{
			const auto exprType = GetExpressionType(*expr, m_visitor);
			const auto [retType, argTypes] = GetFnType(m_fnTypes, currFn);
			CheckType(exprType, { retType });
		}
	}

	void Visit(const IfStatementAST& node) override
	{
		const auto& expr = node.GetExpr();
		const auto exprType = GetExpressionType(expr, m_visitor);

		const auto& thenStmt = node.GetThenStmt();
		WithNewScope([&] {
			thenStmt.Accept(*this); 
		});

		if (const auto& elseStmt = node.GetElseStmt(); elseStmt)
		{
			WithNewScope([&] {
				elseStmt->Accept(*this); 
			});
		}
	}

	void Visit(const WhileStatementAST& node) override
	{
		const auto& expr = node.GetExpr();
		const auto exprType = GetExpressionType(expr, m_visitor);
		CheckType(exprType, { { ExpressionType::Value::Bool } });
		const auto& stmt = node.GetStatement();
		WithNewScope([&] {
			stmt.Accept(*this);
		});
	}

	void Visit(const CompositeStatementAST& node) override
	{
		WithNewScope([&] {
			for (size_t i = 0; i < node.GetCount(); ++i)
			{
				const auto& stmt = node.GetStatement(i);
				stmt.Accept(*this);
			}
		});
	}

	void Visit(const BuiltinCallStatementAST& node) override
	{
		const auto& builtIn = node.GetBuiltin();

		std::vector<ExpressionType> argTypes;
		for (size_t i = 0; i < node.GetParamsCount(); ++i)
		{
			const auto& expr = node.GetExpression(i);
			const auto exprType = GetExpressionType(expr, m_visitor);
			argTypes.push_back(exprType);
		}
		switch (builtIn)
		{
		case BuiltinCallStatementAST::Builtin::Print:
			break; // Any arguments of any types
		case BuiltinCallStatementAST::Builtin::Read:
			if (argTypes.size() < 2)
				throw std::exception("Too less read() arguments count");
			CheckType(argTypes.front(), { { ExpressionType::Value::String } });
			break;
		default:
			throw std::exception("Unknown switch (builtin)");
		}
	}

	void Visit(const FunctionCallStatementAST& node) override
	{
		const auto& call = node.GetCallAsDerived();

		const auto& fnName = call.GetName();
		const auto [retType, expectedArgTypes] = GetFnType(m_fnTypes, fnName);

		std::vector<ExpressionType> argTypes;
		for (size_t i = 0; i < call.GetParamsCount(); ++i)
		{
			const auto& expr = call.GetParam(i);
			const auto exprType = GetExpressionType(expr, m_visitor);
			argTypes.push_back(exprType);
		}
		CheckType(argTypes, expectedArgTypes);
	}

private:
	TypeCheckingExpressionVisitor m_visitor;
	std::map<std::string, std::pair<ExpressionType, std::vector<ExpressionType>>> m_fnTypes;
};

std::map<std::string, std::pair<ExpressionType, std::vector<ExpressionType>>> GetFnTypes(const ProgramAST& ast)
{
	std::map<std::string, std::pair<ExpressionType, std::vector<ExpressionType>>> fnTypes;
	for (size_t i = 0; i < ast.GetFunctionsCount(); ++i)
	{
		const auto& fn = ast.GetFunction(i);
		const auto& retType = fn.GetReturnType();
		const auto& name = fn.GetIdentifier();
		const auto& args = fn.GetParams();

		const auto fnId = name.GetName();
		std::vector<ExpressionType> argTypes = FunctionParamsToTypes(args);

		if (fnTypes.count(fnId))
			throw std::exception("[ERROR] Function redifinition");
		fnTypes[fnId] = std::make_pair(retType, argTypes);
	}

	if (const auto it = std::find_if(fnTypes.cbegin(), fnTypes.cend(), [](auto bla) {
			const auto [key, value] = bla;
			const auto [retType, argTypes] = value;

			if (key != "Main") return false;
			return (((retType == ExpressionType{ ExpressionType::Value::Int }) || (retType == ExpressionType{ ExpressionType::Value::Void }))
				&& (argTypes.empty() || (argTypes == std::vector{ ExpressionType{ ExpressionType::Value::Int }, ExpressionType{ ExpressionType::Value::String, 1 } })));
		}); it == fnTypes.cend()) throw std::exception("[ERROR] No function found with FUNCTION Main");
	return fnTypes;
}

void CheckTypes(const ProgramAST& ast)
{
	auto fnTypes = GetFnTypes(ast);

	TypeCheckingStatementVisitor visitor(fnTypes);
	for (size_t i = 0; i < ast.GetFunctionsCount(); ++i)
	{
		const auto& fn = ast.GetFunction(i);
		const auto& stmt = fn.GetStatement();

		const auto& name = fn.GetIdentifier();
		const auto fnId = name.GetName();

		const auto& args = fn.GetParams();

		currFn = fnId;
		WithNewFunctionScope(args, [&] {
			stmt.Accept(visitor);
		});
	}
}
