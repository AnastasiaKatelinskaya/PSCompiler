#include "ASTBuilder.h"

ASTBuilder::ASTBuilder(const Token& token)
	: m_token(token)
{
}

ProgramAST ASTBuilder::BuildProgramAST()
{
	ProgramAST program;
	for (auto& func : m_functions)
	{
		program.AddFunction(std::move(func));
	}
	return program;
}

void ASTBuilder::ActionOnFunction()
{
	assert(!m_statements.empty());
	assert(!m_expressions.empty());

	auto statement = Pop(m_statements);
	auto identifier = DowncastUniquePtr<IdentifierAST>(Pop(m_expressions));
	assert(identifier);

	auto type = m_functionReturnType;
	m_functionReturnType = std::nullopt;
	auto params = std::move(m_funcProtoParamList);
	m_funcProtoParamList.clear();

	m_functions.push_back(
		std::make_unique<FunctionAST>(
			*type, // TODO: always exists
			std::move(identifier),
			std::move(params),
			std::move(statement)));
}

void ASTBuilder::ActionOnFunctionArgument()
{
	assert(!m_expressions.empty());
	assert(!m_types.empty());

	// Достаем распарсенный идентификатор из стека (если выражение из стека имеет тип не идентификатора, то внутренняя ошибка)
	auto identifier = DowncastUniquePtr<IdentifierAST>(Pop(m_expressions));
	assert(identifier);

	FunctionAST::Param param(identifier->GetName(), Pop(m_types));

	m_funcProtoParamList.push_back(param);
}

void ASTBuilder::ActionOnFunctionReturnType()
{
	assert(!m_types.empty());
	m_functionReturnType = Pop(m_types);
}

void ASTBuilder::ActionOnType(ExpressionType type)
{
	m_types.push_back(type);
}

void ASTBuilder::ActionOnArrayType()
{
	assert(!m_types.empty());
	++m_types.back().nesting;
}

void ASTBuilder::ActionOnIfStatement()
{
	assert(!m_expressions.empty());
	assert(!m_statements.empty());
	auto expr = Pop(m_expressions);
	auto then = Pop(m_statements);
	m_statements.push_back(std::make_unique<IfStatementAST>(std::move(expr), std::move(then)));
}

void ASTBuilder::ActionOnElseStatement()
{
	assert(m_statements.size() >= 2);
	auto stmt = Pop(m_statements);
	auto ifStmt = DowncastUniquePtr<IfStatementAST>(Pop(m_statements));
	ifStmt->SetElseClause(std::move(stmt));
	m_statements.push_back(std::move(ifStmt));
}

void ASTBuilder::ActionOnWhileStatement()
{
	assert(!m_expressions.empty());
	assert(!m_statements.empty());
	auto expr = Pop(m_expressions);
	auto stmt = Pop(m_statements);
	m_statements.push_back(std::make_unique<WhileStatementAST>(
		std::move(expr), std::move(stmt)));
}

void ASTBuilder::ActionOnVariableDeclaration()
{
	// Если при вызове этой функции не были распарсены тип и идентификатор, то внутренняя ошибка
	assert(!m_expressions.empty());
	assert(!m_types.empty());

	// Достаем из стека тип объявляемой переменной
	auto type = Pop(m_types);

	// Достаем из стека идентификатор объявляемой переменной (если тип не IdentifierAST, тогда это внутренняя ошибка)
	auto identifier = DowncastUniquePtr<IdentifierAST>(std::move(Pop(m_expressions)));
	assert(identifier);

	// Создаем узел объявления переменной
	auto node = std::make_unique<VariableDeclarationAST>(std::move(identifier), type);

	// Если был распарсен блок опционального присваивания при объявлении, то достаем выражение и сохраняем его
	if (m_optionalAssignExpression) // TODO: always true but grammar-shit
	{
		node->SetExpression(std::move(m_optionalAssignExpression));
		m_optionalAssignExpression = nullptr;
	}

	// Добавляем узел объявления переменной в стек
	m_statements.push_back(std::move(node));
}

void ASTBuilder::ActionOnAssignStatement()
{
	assert(!m_expressions.empty());
	m_optionalAssignExpression = std::move(Pop(m_expressions));
}

void ASTBuilder::ActionOnAssignStatementAfterIdentifier()
{
	assert(m_expressions.size() >= 2);
	auto expr = Pop(m_expressions);
	auto identifier = DowncastUniquePtr<IdentifierAST>(std::move(Pop(m_expressions)));

	m_statements.push_back(std::make_unique<AssignStatementAST>(
		std::move(identifier), std::move(expr)));
}

void ASTBuilder::ActionOnAssignStatementArrayElement()
{
	assert(m_expressions.size() >= 2);

	auto expression = Pop(m_expressions);
	auto access = DowncastUniquePtr<ArrayElementAccessAST>(Pop(m_expressions));

	assert(access);
	m_statements.push_back(std::make_unique<ArrayElementAssignAST>(std::move(access), std::move(expression)));
}

void ASTBuilder::ActionOnPrepareFunctionCallArgument()
{
	m_functionCallParamList.emplace_back();
}

void ASTBuilder::ActionOnFunctionCallArgument()
{
	assert(!m_expressions.empty());
	assert(!m_functionCallParamList.empty());

	auto& params = m_functionCallParamList.back();
	params.push_back(Pop(m_expressions));
}

void ASTBuilder::ActionOnPrepareArrayLiteralElements()
{
	m_arrayLiteralElementsList.emplace_back();
}

void ASTBuilder::ActionOnArrayLiteralConstant()
{
	assert(!m_arrayLiteralElementsList.empty());
	std::vector<std::shared_ptr<IExpressionAST>> expressions;
	for (auto& expression : m_arrayLiteralElementsList.back())
	{
		expressions.push_back(std::move(expression));
	}
	m_arrayLiteralElementsList.pop_back();
	m_expressions.push_back(std::make_unique<LiteralConstantAST>(std::move(expressions)));
}

void ASTBuilder::ActionOnArrayExpression()
{
	assert(!m_expressions.empty());
	assert(!m_arrayLiteralElementsList.empty());
	m_arrayLiteralElementsList.back().push_back(Pop(m_expressions));
}

void ASTBuilder::ActionOnFunctionCallStatement()
{
	auto call = CreateFunctionCallExpressionAST();
	m_statements.push_back(std::make_unique<FunctionCallStatementAST>(std::move(call)));
}

void ASTBuilder::ActionOnReturnStatement()
{
	if (m_optionalReturnExpression)
	{
		m_statements.push_back(std::make_unique<ReturnStatementAST>(std::move(m_optionalReturnExpression)));
		m_optionalReturnExpression = nullptr;
	}
	else
	{
		m_statements.push_back(std::make_unique<ReturnStatementAST>(nullptr));
	}
}

void ASTBuilder::ActionOnReturnExpression()
{
	assert(!m_expressions.empty());
	auto expression = Pop(m_expressions);
	m_optionalReturnExpression = std::move(expression);
}

void ASTBuilder::ActionOnPrepareCompositeStatement()
{
	m_compositeCache.emplace_back();
}

void ASTBuilder::ActionOnCompositeStatement()
{
	auto composite = std::make_unique<CompositeStatementAST>();
	for (auto& stmt : m_compositeCache.back())
	{
		composite->AddStatement(std::move(stmt));
	}
	m_compositeCache.pop_back();
	m_statements.push_back(std::move(composite));
}

void ASTBuilder::ActionOnCompositeStatementPart()
{
	assert(!m_statements.empty());
	m_compositeCache.back().push_back(Pop(m_statements));
}

void ASTBuilder::ActionOnPrintStatement()
{
	assert(!m_functionCallParamList.empty());
	auto printStatement = std::make_unique<BuiltinCallStatementAST>(BuiltinCallStatementAST::Builtin::Print);

	for (auto& expr : m_functionCallParamList.back())
	{
		printStatement->AddExpression(std::move(expr));
	}

	m_functionCallParamList.pop_back();
	m_statements.push_back(std::move(printStatement));
}

void ASTBuilder::ActionOnReadStatement()
{
	assert(!m_functionCallParamList.empty());
	auto printStatement = std::make_unique<BuiltinCallStatementAST>(BuiltinCallStatementAST::Builtin::Read);

	for (auto& expr : m_functionCallParamList.back())
	{
		printStatement->AddExpression(std::move(expr));
	}

	m_functionCallParamList.pop_back();
	m_statements.push_back(std::move(printStatement));
}

void ASTBuilder::ActionOnBinaryExpression(BinaryExpressionAST::Operator op)
{
	assert(m_expressions.size() >= 2);

	auto right = Pop(m_expressions);
	auto left = Pop(m_expressions);

	m_expressions.push_back(std::make_unique<BinaryExpressionAST>(std::move(left), std::move(right), op));
}

void ASTBuilder::ActionOnIdentifier()
{
	assert(m_token.type == TokenType::Identifier);
	m_expressions.push_back(std::make_unique<IdentifierAST>(*m_token.value));
}

void ASTBuilder::ActionOnIntConstant()
{
	assert(m_token.type == TokenType::IntConstant);
	m_expressions.push_back(std::make_unique<LiteralConstantAST>(std::stoi(*m_token.value)));
}

void ASTBuilder::ActionOnFloatConstant()
{
	assert(m_token.type == TokenType::FloatConstant);
	m_expressions.push_back(std::make_unique<LiteralConstantAST>(std::stod(*m_token.value)));
}

void ASTBuilder::ActionOnTrueConstant()
{
	assert(m_token.type == TokenType::True);
	auto trueConstantLiteral = std::make_unique<LiteralConstantAST>(true);
	m_expressions.push_back(std::move(trueConstantLiteral));
}

void ASTBuilder::ActionOnFalseConstant()
{
	assert(m_token.type == TokenType::False);
	auto falseConstantLiteral = std::make_unique<LiteralConstantAST>(false);
	m_expressions.push_back(std::move(falseConstantLiteral));
}

void ASTBuilder::ActionOnStringConstant()
{
	assert(m_token.type == TokenType::StringConstant);
	auto stringConstantLiteral = std::make_unique<LiteralConstantAST>(*m_token.value);
	m_expressions.push_back(std::move(stringConstantLiteral));
}

void ASTBuilder::ActionOnAccessingArrayElement()
{
	assert(m_expressions.size() >= 2);

	auto index = Pop(m_expressions);
	auto identifier = DowncastUniquePtr<IdentifierAST>(Pop(m_expressions));

	assert(identifier);
	m_expressions.push_back(std::make_unique<ArrayElementAccessAST>(identifier->GetName(), std::move(index)));
}

void ASTBuilder::ActionOnAccessingAdditionalSquareBracket()
{
	assert(m_expressions.size() >= 2);

	auto expression = Pop(m_expressions);
	auto access = DowncastUniquePtr<ArrayElementAccessAST>(Pop(m_expressions));

	assert(access);
	access->AddIndex(std::move(expression));

	m_expressions.push_back(std::move(access));
}

void ASTBuilder::ActionOnUnaryMinus()
{
	assert(!m_expressions.empty());
	auto node = std::move(m_expressions.back());
	m_expressions.pop_back();
	m_expressions.push_back(std::make_unique<UnaryAST>(std::move(node), UnaryAST::Operator::Minus));
}

void ASTBuilder::ActionOnUnaryPlus()
{
	assert(!m_expressions.empty());
	auto node = std::move(m_expressions.back());
	m_expressions.pop_back();
	m_expressions.push_back(std::make_unique<UnaryAST>(std::move(node), UnaryAST::Operator::Plus));
}

void ASTBuilder::ActionOnUnaryNegation()
{
	assert(!m_expressions.empty());
	auto node = std::move(m_expressions.back());
	m_expressions.pop_back();
	m_expressions.push_back(std::make_unique<UnaryAST>(std::move(node), UnaryAST::Operator::Negation));
}

void ASTBuilder::ActionOnFunctionCallExpression()
{
	auto funcCall = CreateFunctionCallExpressionAST();
	m_expressions.push_back(std::move(funcCall));
}

std::unique_ptr<FunctionCallExpressionAST> ASTBuilder::CreateFunctionCallExpressionAST()
{
	assert(!m_expressions.empty());

	auto identifier = DowncastUniquePtr<IdentifierAST>(Pop(m_expressions));
	assert(identifier);

	assert(!m_functionCallParamList.empty());
	std::vector<std::unique_ptr<IExpressionAST>> expressions;

	for (auto& expr : m_functionCallParamList.back())
	{
		expressions.push_back(std::move(expr));
	}

	m_functionCallParamList.pop_back();
	return std::make_unique<FunctionCallExpressionAST>(identifier->GetName(), std::move(expressions));
}
