#pragma once
#include "../Lexer/Token.h"
#include "AST.h"
#include "Entity/ArrayElementAccessAST.h"
#include "Entity/ArrayElementAssignAST.h"
#include "Entity/AssignStatementAST.h"
#include "Entity/BinaryExpressionAST.h"
#include "Entity/BuiltinCallStatementAST.h"
#include "Entity/CompositeStatementAST.h"
#include "Entity/FunctionAST.h"
#include "Entity/FunctionCallExpressionAST.h"
#include "Entity/FunctionCallStatementAST.h"
#include "Entity/IdentifierAST.h"
#include "Entity/IfStatementAST.h"
#include "Entity/LiteralConstantAST.h"
#include "Entity/ProgramAST.h"
#include "Entity/ReturnStatementAST.h"
#include "Entity/UnaryAST.h"
#include "Entity/VariableDeclarationAST.h"
#include "Entity/WhileStatementAST.h"

#include <cassert>

template <typename T>
T Pop(std::vector<T>& vect)
{
	auto value = std::move(vect.back());
	vect.pop_back();
	return std::move(value);
}

template <typename Derived, typename Base>
std::unique_ptr<Derived> DowncastUniquePtr(std::unique_ptr<Base>&& base)
{
	std::unique_ptr<Derived> derived = nullptr;
	if (Derived* ptr = dynamic_cast<Derived*>(base.get()))
	{
		base.release();
		derived.reset(ptr);
		return std::move(derived);
	}
	assert(false);
	return nullptr;
}

class ASTBuilder
{
public:
	ASTBuilder(const Token& token);

	ProgramAST BuildProgramAST();

	void ActionOnFunction();
	void ActionOnFunctionArgument();
	void ActionOnFunctionReturnType();
	void ActionOnType(ExpressionType type);
	void ActionOnArrayType();
	void ActionOnIfStatement();
	void ActionOnElseStatement();
	void ActionOnWhileStatement();
	void ActionOnVariableDeclaration();
	void ActionOnAssignStatement();
	void ActionOnAssignStatementAfterIdentifier();
	void ActionOnAssignStatementArrayElement();
	void ActionOnPrepareFunctionCallArgument();
	void ActionOnFunctionCallArgument();
	void ActionOnPrepareArrayLiteralElements();
	void ActionOnArrayLiteralConstant();
	void ActionOnArrayExpression();
	void ActionOnFunctionCallStatement();
	void ActionOnReturnStatement();
	void ActionOnReturnExpression();
	void ActionOnPrepareCompositeStatement();
	void ActionOnCompositeStatement();
	void ActionOnCompositeStatementPart();
	void ActionOnPrintStatement();
	void ActionOnReadStatement();
	void ActionOnBinaryExpression(BinaryExpressionAST::Operator op);
	void ActionOnIdentifier();
	void ActionOnIntConstant();
	void ActionOnFloatConstant();
	void ActionOnTrueConstant();
	void ActionOnFalseConstant();
	void ActionOnStringConstant();
	void ActionOnAccessingArrayElement();
	void ActionOnAccessingAdditionalSquareBracket();
	void ActionOnUnaryMinus();
	void ActionOnUnaryPlus();
	void ActionOnUnaryNegation();
	void ActionOnFunctionCallExpression();

private:
	std::unique_ptr<FunctionCallExpressionAST> CreateFunctionCallExpressionAST();

private:
	// Текущий токен, который был считан лексером
	const Token& m_token;

	// Стек для временного хранения считанных типов
	std::vector<ExpressionType> m_types;

	// Стек для временного хранения считанных параметров узла объявления функции
	std::vector<FunctionAST::Param> m_funcProtoParamList;

	// Стек для постепенного создания AST выражений
	std::vector<std::unique_ptr<IExpressionAST>> m_expressions;

	// Вспомогательный стек для хранения параметров вызова функции
	std::vector<std::vector<std::unique_ptr<IExpressionAST>>> m_functionCallParamList;

	std::vector<std::vector<std::unique_ptr<IExpressionAST>>> m_arrayLiteralElementsList;

	// Если был распарсен опциональный тип возвращаемого значения функции, эта переменная будет не пуста
	std::optional<ExpressionType> m_functionReturnType;

	// Если был распарсен опциональный блок присваивания при объявлении, эта переменная будет не пуста
	std::unique_ptr<IExpressionAST> m_optionalAssignExpression;

	// Если было распарсено опциональное выражение возврата из функции, эта переменная будет не пуста
	std::unique_ptr<IExpressionAST> m_optionalReturnExpression;

	// Стек для постепенного создания AST инструкций
	std::vector<std::unique_ptr<IStatementAST>> m_statements;

	// Стек для временного хранения узлов AST вложенных инструкций
	std::vector<std::vector<std::unique_ptr<IStatementAST>>> m_compositeCache;

	// Стек для хранения AST функций
	std::vector<std::unique_ptr<FunctionAST>> m_functions;
};