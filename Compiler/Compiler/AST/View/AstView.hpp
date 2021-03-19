#pragma once

#include "DotBuilder.hpp"

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

#include <stack>

DotBuilder dotBuilder;
std::stack<size_t> vertices;

template <class T>
void WithVertex(size_t vertexId, T&& fn)
{
	vertices.push(vertexId);
	fn();
	vertices.pop();
}

class ExpressionVisitor : public IExpressionVisitor
{
public:
	void Visit(const BinaryExpressionAST& node) override
	{
		const auto topId = vertices.top();
		const auto binExprId = dotBuilder.AddVertex("binExpr");
		dotBuilder.AddEdge(topId, binExprId);

		const auto& left = node.GetLeft();
		const auto& oper = node.GetOperator();
		const auto& right = node.GetRight();

		WithVertex(binExprId, [&] {
			left.Accept(*this);

			const auto operId = dotBuilder.AddVertex(ToString(oper));
			dotBuilder.AddEdge(binExprId, operId);

			right.Accept(*this);
		});
	}

	void Visit(const LiteralConstantAST& node) override
	{
		const auto topId = vertices.top();

		const auto& value = node.GetValue();
		const auto valId = std::visit([this](auto&& arg) -> size_t {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (false
				|| std::is_same_v<T, int>
				|| std::is_same_v<T, double>
				|| std::is_same_v<T, bool>
				)
				return dotBuilder.AddVertex(std::to_string(arg));
			else if constexpr (std::is_same_v<T, std::string>)
				return dotBuilder.AddVertex(arg);
			else if constexpr (std::is_same_v<T, std::vector<std::shared_ptr<IExpressionAST>>>) {
				const auto arrValsId = dotBuilder.AddVertex("[...]");
				WithVertex(arrValsId, [&] {
					for (const auto& val : arg) {
						val->Accept(*this);
					}
				});
				return arrValsId;
			}
			else
				static_assert(always_false_v<T>, "Non-exhaustive visitor!");
			}, value);
		dotBuilder.AddEdge(topId, valId);
	}

	void Visit(const UnaryAST& node) override
	{
		const auto topId = vertices.top();
		const auto unaryId = dotBuilder.AddVertex("unaryOp");
		dotBuilder.AddEdge(topId, unaryId);

		const auto& oper = node.GetOperator();
		const auto operId = dotBuilder.AddVertex(ToString(oper));
		dotBuilder.AddEdge(unaryId, operId);

		const auto& expr = node.GetExpr();
		WithVertex(unaryId, [&] {
			expr.Accept(*this);
		});
	}

	void Visit(const IdentifierAST& node) override
	{
		const auto topId = vertices.top();

		const auto idId = dotBuilder.AddVertex(node.GetName());
		dotBuilder.AddEdge(topId, idId);
	}

	void Visit(const FunctionCallExpressionAST& node) override
	{
		const auto topId = vertices.top();

		const auto fnCallId = dotBuilder.AddVertex(node.GetName() + "(...)");
		dotBuilder.AddEdge(topId, fnCallId);
		WithVertex(fnCallId, [&] {
			for (size_t i = 0; i < node.GetParamsCount(); ++i)
			{
				const auto& expr = node.GetParam(i);
				expr.Accept(*this);
			}
		});
	}

	void Visit(const ArrayElementAccessAST& node) override
	{
		const auto topId = vertices.top();
		const auto arrAccessId = dotBuilder.AddVertex("arr[...]");
		dotBuilder.AddEdge(topId, arrAccessId);

		const auto arrNameId = dotBuilder.AddVertex(node.GetName());
		dotBuilder.AddEdge(arrAccessId, arrNameId);

		const auto arrIndexId = dotBuilder.AddVertex("[...]");
		dotBuilder.AddEdge(arrAccessId, arrIndexId);
		WithVertex(arrIndexId, [&] {
			for (size_t i = 0; i < node.GetIndexCount(); ++i)
			{
				const auto& index = node.GetIndex(i);
				index.Accept(*this);
			}
		});
	}
};

class StatementVisitor : public IStatementVisitor
{
public:
	void Visit(const VariableDeclarationAST& node) override
	{
		const auto topId = vertices.top();

		const auto varDecId = dotBuilder.AddVertex("varDec");
		dotBuilder.AddEdge(topId, varDecId);

		const auto typeId = dotBuilder.AddVertex(ToString(node.GetType()));
		dotBuilder.AddEdge(varDecId, typeId);

		WithVertex(varDecId, [&] {
			const auto& id = node.GetIdentifier();
			id.Accept(m_visitor);
		});

		const auto& expr = node.GetExpression();
		WithVertex(varDecId, [&] {
			expr.Accept(m_visitor);
		});
	}

	void Visit(const AssignStatementAST& node) override
	{
		const auto topId = vertices.top();
		const auto assignId = dotBuilder.AddVertex("=");
		dotBuilder.AddEdge(topId, assignId);

		const auto idId = dotBuilder.AddVertex(node.GetIdentifier().GetName());
		dotBuilder.AddEdge(assignId, idId);

		WithVertex(assignId, [&] {
			const auto& expr = node.GetExpr();
			expr.Accept(m_visitor);
		});
	}

	void Visit(const ArrayElementAssignAST& node) override
	{
		const auto topId = vertices.top();
		const auto arrElAssign = dotBuilder.AddVertex("arr[...]=");
		dotBuilder.AddEdge(topId, arrElAssign);

		const auto arrNameId = dotBuilder.AddVertex(node.GetName());
		dotBuilder.AddEdge(arrElAssign, arrNameId);

		const auto arrIndexId = dotBuilder.AddVertex("[...]");
		dotBuilder.AddEdge(arrElAssign, arrIndexId);
		WithVertex(arrIndexId, [&] {
			for (size_t i = 0; i < node.GetIndexCount(); ++i)
			{
				const auto& index = node.GetIndex(i);
				index.Accept(m_visitor);
			}
		});

		WithVertex(arrElAssign, [&] {
			const auto& expr = node.GetExpression();
			expr.Accept(m_visitor);
		});
	}

	void Visit(const ReturnStatementAST& node) override
	{
		const auto topId = vertices.top();
		const auto retId = dotBuilder.AddVertex("return");
		dotBuilder.AddEdge(topId, retId);
		if (const auto& expr = node.GetExpression(); expr)
		{
			WithVertex(retId, [&] {
				expr->Accept(m_visitor);
			});
		}
	}

	void Visit(const IfStatementAST& node) override
	{
		const auto topId = vertices.top();
		const auto ifId = dotBuilder.AddVertex("if");
		dotBuilder.AddEdge(topId, ifId);

		WithVertex(ifId, [&] {
			const auto& expr = node.GetExpr();
			expr.Accept(m_visitor);
		});

		WithVertex(ifId, [&] {
			const auto& thenStmt = node.GetThenStmt();
			thenStmt.Accept(*this);
		});

		if (const auto& elseStmt = node.GetElseStmt(); elseStmt)
		{
			WithVertex(ifId, [&] {
				elseStmt->Accept(*this);
			});
		}
	}

	void Visit(const WhileStatementAST& node) override
	{
		const auto topId = vertices.top();
		const auto whileId = dotBuilder.AddVertex("while");
		dotBuilder.AddEdge(topId, whileId);

		WithVertex(whileId, [&] {
			const auto& expr = node.GetExpr();
			expr.Accept(m_visitor);
		});

		WithVertex(whileId, [&] {
			const auto& stmt = node.GetStatement();
			stmt.Accept(*this);
		});
	}

	void Visit(const CompositeStatementAST& node) override
	{
		for (size_t i = 0; i < node.GetCount(); ++i)
		{
			const auto& stmt = node.GetStatement(i);
			stmt.Accept(*this);
		}
	}

	void Visit(const BuiltinCallStatementAST& node) override
	{
		const auto topId = vertices.top();

		const auto& builtIn = node.GetBuiltin();
		const auto builtInId = dotBuilder.AddVertex(ToString(builtIn) + "(...)");
		dotBuilder.AddEdge(topId, builtInId);

		for (size_t i = 0; i < node.GetParamsCount(); ++i)
		{
			WithVertex(builtInId, [&] {
				const auto& expr = node.GetExpression(i);
				expr.Accept(m_visitor);
			});
		}
	}

	void Visit(const FunctionCallStatementAST& node) override
	{
		const auto topId = vertices.top();

		const auto& call = node.GetCallAsDerived();
		const auto fnId = dotBuilder.AddVertex(call.GetName() + "(...)");
		dotBuilder.AddEdge(topId, fnId);

		for (size_t i = 0; i < call.GetParamsCount(); ++i)
		{
			WithVertex(fnId, [&] {
				const auto& param = call.GetParam(i);
				param.Accept(m_visitor);
			});
		}
	}

private:
	ExpressionVisitor m_visitor;
};

void BuildAstView(const ProgramAST& ast)
{
	const auto programId = dotBuilder.AddVertex("<PROGRAM>");
	for (size_t i = 0; i < ast.GetFunctionsCount(); ++i)
	{
		StatementVisitor statementVisitor;
		ExpressionVisitor expressionVisitor;

		const auto& fn = ast.GetFunction(i);

		const auto& id = fn.GetIdentifier();
		const auto fnId = dotBuilder.AddVertex(id.GetName());
		dotBuilder.AddEdge(programId, fnId);

		const auto& ret = fn.GetReturnType();
		const auto retId = dotBuilder.AddVertex(ToString(ret));
		dotBuilder.AddEdge(fnId, retId);

		const auto argsId = dotBuilder.AddVertex("args");
		dotBuilder.AddEdge(fnId, argsId);
		for (const auto& [name, type] : fn.GetParams())
		{
			const auto argId = dotBuilder.AddVertex("arg");
			dotBuilder.AddEdge(argsId, argId);

			const auto typeId = dotBuilder.AddVertex(ToString(type));
			const auto nameId = dotBuilder.AddVertex(name);
			dotBuilder.AddEdge(argId, typeId);
			dotBuilder.AddEdge(argId, nameId);
		}

		const auto statemtnsId = dotBuilder.AddVertex("statements");
		dotBuilder.AddEdge(fnId, statemtnsId);
		WithVertex(statemtnsId, [&] {
			const auto& stmt = fn.GetStatement();
			stmt.Accept(statementVisitor);
		});
	}
}
