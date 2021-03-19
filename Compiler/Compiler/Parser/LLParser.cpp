#include "LLParser.h"

LLParser::LLParser(
	std::unique_ptr<ILexer>&& lexer,
	std::unique_ptr<LLParserTable>&& table,
	std::ostream& output)
	: m_lexer(std::move(lexer))
	, m_table(std::move(table))
	, m_output(output)
{
}

ProgramAST LLParser::Parse(const std::string& text)
{
	m_lexer->SetText(text);
	Token token = m_lexer->GetNextToken();

	std::vector<size_t> addresses;
	size_t index = 0;

	ASTBuilder astBuilder(token);
	std::unordered_map<std::string, std::function<void()>> actions = {
		{ "ActionOnFunctionCallStatement", std::bind(&ASTBuilder::ActionOnFunctionCallStatement, &astBuilder) },
		{ "ActionOnFunctionCallArgument", std::bind(&ASTBuilder::ActionOnFunctionCallArgument, &astBuilder) },
		{ "ActionOnFunction", std::bind(&ASTBuilder::ActionOnFunction, &astBuilder) },
		{ "ActionOnFunctionReturnType", std::bind(&ASTBuilder::ActionOnFunctionReturnType, &astBuilder) },
		{ "ActionOnFunctionArgument", std::bind(&ASTBuilder::ActionOnFunctionArgument, &astBuilder) },
		{ "ActionOnFunctionCallExpression", std::bind(&ASTBuilder::ActionOnFunctionCallExpression, &astBuilder) },
		{ "ActionOnPrepareFunctionCallArgument", std::bind(&ASTBuilder::ActionOnPrepareFunctionCallArgument, &astBuilder) },

		{ "ActionOnIfStatement", std::bind(&ASTBuilder::ActionOnIfStatement, &astBuilder) },
		{ "ActionOnElseStatement", std::bind(&ASTBuilder::ActionOnElseStatement, &astBuilder) },
		{ "ActionOnWhileStatement", std::bind(&ASTBuilder::ActionOnWhileStatement, &astBuilder) },
		{ "ActionOnVariableDeclaration", std::bind(&ASTBuilder::ActionOnVariableDeclaration, &astBuilder) },
		{ "ActionOnPrintStatement", std::bind(&ASTBuilder::ActionOnPrintStatement, &astBuilder) },
		{ "ActionOnReadStatement", std::bind(&ASTBuilder::ActionOnReadStatement, &astBuilder) },

		{ "ActionOnAssignStatement", std::bind(&ASTBuilder::ActionOnAssignStatement, &astBuilder) },
		{ "ActionOnAssignStatementAfterIdentifier", std::bind(&ASTBuilder::ActionOnAssignStatementAfterIdentifier, &astBuilder) },
		{ "ActionOnAssignStatementArrayElement", std::bind(&ASTBuilder::ActionOnAssignStatementArrayElement, &astBuilder) },

		{ "ActionOnReturnStatement", std::bind(&ASTBuilder::ActionOnReturnStatement, &astBuilder) },
		{ "ActionOnReturnExpression", std::bind(&ASTBuilder::ActionOnReturnExpression, &astBuilder) },

		{ "ActionOnPrepareCompositeStatement", std::bind(&ASTBuilder::ActionOnPrepareCompositeStatement, &astBuilder) },
		{ "ActionOnCompositeStatement", std::bind(&ASTBuilder::ActionOnCompositeStatement, &astBuilder) },
		{ "ActionOnCompositeStatementPart", std::bind(&ASTBuilder::ActionOnCompositeStatementPart, &astBuilder) },

		{ "ActionOnVoidType", std::bind(&ASTBuilder::ActionOnType, &astBuilder, ExpressionType{ ExpressionType::Value::Void, 0 }) },
		{ "ActionOnIntType", std::bind(&ASTBuilder::ActionOnType, &astBuilder, ExpressionType{ ExpressionType::Value::Int, 0 }) },
		{ "ActionOnFloatType", std::bind(&ASTBuilder::ActionOnType, &astBuilder, ExpressionType{ ExpressionType::Value::Float, 0 }) },
		{ "ActionOnBoolType", std::bind(&ASTBuilder::ActionOnType, &astBuilder, ExpressionType{ ExpressionType::Value::Bool, 0 }) },
		{ "ActionOnStringType", std::bind(&ASTBuilder::ActionOnType, &astBuilder, ExpressionType{ ExpressionType::Value::String, 0 }) },
		{ "ActionOnArrayType", std::bind(&ASTBuilder::ActionOnArrayType, &astBuilder) },

		{ "ActionOnBinaryOr", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::Or) },
		{ "ActionOnBinaryAnd", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::And) },
		{ "ActionOnBinaryEquals", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::Equals) },
		{ "ActionOnBinaryNotEquals", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::NotEquals) },
		{ "ActionOnBinaryLess", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::Less) },
		{ "ActionOnBinaryLessOrEquals", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::LessOrEquals) },
		{ "ActionOnBinaryMore", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::More) },
		{ "ActionOnBinaryMoreOrEquals", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::MoreOrEquals) },
		{ "ActionOnBinaryPlus", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::Plus) },
		{ "ActionOnBinaryMinus", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::Minus) },
		{ "ActionOnBinaryMultiplication", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::Multiplication) },
		{ "ActionOnBinaryDivision", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::Division) },
		{ "ActionOnBinaryModulo", std::bind(&ASTBuilder::ActionOnBinaryExpression, &astBuilder, BinaryExpressionAST::Operator::Modulo) },

		{ "ActionOnIdentifier", std::bind(&ASTBuilder::ActionOnIdentifier, &astBuilder) },
		{ "ActionOnIntConstant", std::bind(&ASTBuilder::ActionOnIntConstant, &astBuilder) },
		{ "ActionOnFloatConstant", std::bind(&ASTBuilder::ActionOnFloatConstant, &astBuilder) },
		{ "ActionOnTrueConstant", std::bind(&ASTBuilder::ActionOnTrueConstant, &astBuilder) },
		{ "ActionOnFalseConstant", std::bind(&ASTBuilder::ActionOnFalseConstant, &astBuilder) },
		{ "ActionOnStringConstant", std::bind(&ASTBuilder::ActionOnStringConstant, &astBuilder) },

		{ "ActionOnUnaryMinus", std::bind(&ASTBuilder::ActionOnUnaryMinus, &astBuilder) },
		{ "ActionOnUnaryPlus", std::bind(&ASTBuilder::ActionOnUnaryPlus, &astBuilder) },
		{ "ActionOnUnaryNegation", std::bind(&ASTBuilder::ActionOnUnaryNegation, &astBuilder) },

		{ "ActionOnAccessingAdditionalSquareBracket", std::bind(&ASTBuilder::ActionOnAccessingAdditionalSquareBracket, &astBuilder) },
		{ "ActionOnAccessingArrayElement", std::bind(&ASTBuilder::ActionOnAccessingArrayElement, &astBuilder) },
		{ "ActionOnPrepareArrayLiteralElements", std::bind(&ASTBuilder::ActionOnPrepareArrayLiteralElements, &astBuilder) },
		{ "ActionOnArrayLiteralConstant", std::bind(&ASTBuilder::ActionOnArrayLiteralConstant, &astBuilder) },
		{ "ActionOnArrayExpression", std::bind(&ASTBuilder::ActionOnArrayExpression, &astBuilder) },
	};

	while (true)
	{
		auto state = m_table->GetEntry(index);

		if (state->isAttribute)
		{
			auto it = actions.find(state->name);
			if (it != actions.end())
			{
				it->second();
			}
			else
			{
				throw std::logic_error("attribute '" + state->name + "' doesn't have associated action");
			}
		}
		else if (!EntryAcceptsTerminal(*state, TokenTypeToString(token.type)))
		{
			if (!state->isError)
			{
				++index;
				continue;
			}
			else
			{
				throw std::runtime_error("unexpected token " + TokenTypeToString(token.type) + " found at " + std::to_string(token.line) + ":" + std::to_string(token.column) + ". Maybe u wanted to use this: " + *state->beginnings.begin());
			}
		}

		if (state->isEnding)
		{
			assert(addresses.empty());
			return astBuilder.BuildProgramAST();
		}
		if (state->doPush)
		{
			addresses.push_back(index + 1);
		}
		if (state->doShift)
		{
			token = m_lexer->GetNextToken();
		}

		if (bool(state->next))
		{
			index = *state->next;
		}
		else
		{
			assert(!addresses.empty());
			index = addresses.back();
			addresses.pop_back();
		}
	}

	assert(false);
	throw std::logic_error("parse method of parser doesn't have a break statement in while loop");
}
