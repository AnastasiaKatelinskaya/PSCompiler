#include "CompilerDriver.h"

#include "../AST/Visitor/IStatementVisitor.h"
#include "../AST/Visitor/IExpressionVisitor.h"
#include "../AST/IStatementAST.h"
#include "../AST/IExpressionAST.h"

#include "../AST/Typing/Typing.hpp"
#include "../AST/View/AstView.hpp"

#include <stack>
#include <vector>
#include <memory>

CompilerDriver::CompilerDriver(std::ostream& log)
	: m_log(log)
{
}

void CompilerDriver::Compile(const std::string& text)
{
	auto parser = CreateParser();
	auto ast = parser.Parse(text);

	BuildAstView(ast);
	CheckTypes(ast);
}

bool CompilerDriver::VerifyGrammarTerminalsMatchLexerTokens(const Grammar& grammar, std::string& unmatch)
{
	for (size_t row = 0; row < grammar.GetProductionsCount(); ++row)
	{
		const auto production = grammar.GetProduction(row);
		for (size_t col = 0; col < production->GetSymbolsCount(); ++col)
		{
			const GrammarSymbol& symbol = production->GetSymbol(col);
			if (symbol.GetType() == GrammarSymbolType::Terminal && !TokenTypeExists(symbol.GetText()))
			{
				unmatch = symbol.GetText();
				return false;
			}
		}
	}
	return true;
}

LLParser CompilerDriver::CreateParser()
{
	auto grammar = GrammarBuilder(std::make_unique<GrammarProductionFactory>())
					   .AddProduction("<Program> => <Functions> EOF")

					   .AddProduction("<Functions> => <Function> <Functions>")
					   .AddProduction("<Functions> => $")

					   .AddProduction("<Function> => <FunctionReturnType> <Identifier> LeftParenthesis <FunctionArguments> RightParenthesis Semicolon <Statement> {ActionOnFunction}")

					   .AddProduction("<FunctionReturnType> => <FunctionType> {ActionOnFunctionReturnType}")

					   .AddProduction("<FunctionType> => <Type>")
					   .AddProduction("<FunctionType> => Void {ActionOnVoidType}")

					   .AddProduction("<Type> => INTEGER {ActionOnIntType}")
					   .AddProduction("<Type> => Float {ActionOnFloatType}")
					   .AddProduction("<Type> => Bool {ActionOnBoolType}")
					   .AddProduction("<Type> => String {ActionOnStringType}")
					   .AddProduction("<Type> => Array LeftAngleBrace <Type> RightAngleBrace {ActionOnArrayType}")

					   .AddProduction("<Identifier> => Identifier {ActionOnIdentifier}")

					   .AddProduction("<FunctionArguments> => <FunctionArgument> <FunctionArgumentsTail>")
					   .AddProduction("<FunctionArguments> => $")

					   .AddProduction("<FunctionArgument> => <Type> <Identifier> {ActionOnFunctionArgument}")

					   .AddProduction("<FunctionArgumentsTail> => Comma <FunctionArgument> <FunctionArgumentsTail>")
					   .AddProduction("<FunctionArgumentsTail> => $")

					   .AddProduction("<Statement> => <Condition>")
					   .AddProduction("<Statement> => <Declaration>")
					   .AddProduction("<Statement> => <Cycle>")
					   .AddProduction("<Statement> => <Print>")
					   .AddProduction("<Statement> => <Read>")
					   .AddProduction("<Statement> => <Return>")
					   .AddProduction("<Statement> => <CompositeStatement>")
					   .AddProduction("<Statement> => <Identifier> <StatementAfterIdentifier>")

					   .AddProduction("<Condition> => If LeftParenthesis <Expression> RightParenthesis <Statement> {ActionOnIfStatement} <ElseStatement>")

					   .AddProduction("<ElseStatement> => Else <Statement> {ActionOnElseStatement}")
					   .AddProduction("<ElseStatement> => $")

					   .AddProduction("<Expression> => <OrExpression>")

					   .AddProduction("<OrExpression> => <AndExpression> <OrExpressionTail>")

					   .AddProduction("<OrExpressionTail> => Or <AndExpression> {ActionOnBinaryOr} <OrExpressionTail>")
					   .AddProduction("<OrExpressionTail> => $")

					   .AddProduction("<AndExpression> => <EqualsExpression> <AndExpressionTail>")

					   .AddProduction("<AndExpressionTail> => And <EqualsExpression> {ActionOnBinaryAnd} <AndExpressionTail>")
					   .AddProduction("<AndExpressionTail> => $")

					   .AddProduction("<EqualsExpression> => <LessThanExpression> <EqualsExpressionTail>")

					   .AddProduction("<EqualsExpressionTail> => Equals <LessThanExpression> {ActionOnBinaryEquals} <EqualsExpressionTail>")
					   .AddProduction("<EqualsExpressionTail> => NotEquals <LessThanExpression> {ActionOnBinaryNotEquals} <EqualsExpressionTail>")
					   .AddProduction("<EqualsExpressionTail> => $")

					   .AddProduction("<LessThanExpression> => <SubExpression> <LessThanExpressionTail>")

					   .AddProduction("<LessThanExpressionTail> => LeftAngleBrace <SubExpression> {ActionOnBinaryLess} <LessThanExpressionTail>")
					   .AddProduction("<LessThanExpressionTail> => RightAngleBrace <SubExpression> {ActionOnBinaryMore} <LessThanExpressionTail>")
					   .AddProduction("<LessThanExpressionTail> => LessOrEquals <SubExpression> {ActionOnBinaryLessOrEquals} <LessThanExpressionTail>")
					   .AddProduction("<LessThanExpressionTail> => MoreOrEquals <SubExpression> {ActionOnBinaryMoreOrEquals} <LessThanExpressionTail>")
					   .AddProduction("<LessThanExpressionTail> => $")

					   .AddProduction("<SubExpression> => <DivisionAndMultiplicationExpression> <SubExpressionTail>")

					   .AddProduction("<SubExpressionTail> => Plus <DivisionAndMultiplicationExpression> {ActionOnBinaryPlus} <SubExpressionTail>")
					   .AddProduction("<SubExpressionTail> => Minus <DivisionAndMultiplicationExpression> {ActionOnBinaryMinus} <SubExpressionTail>")
					   .AddProduction("<SubExpressionTail> => $")

					   .AddProduction("<DivisionAndMultiplicationExpression> => <ExpressionItem> <DivisionAndMultiplicationExpressionTail>")

					   .AddProduction("<DivisionAndMultiplicationExpressionTail> => Multiplication <ExpressionItem> {ActionOnBinaryMultiplication} <DivisionAndMultiplicationExpressionTail>")
					   .AddProduction("<DivisionAndMultiplicationExpressionTail> => Division <ExpressionItem> {ActionOnBinaryDivision} <DivisionAndMultiplicationExpressionTail>")
					   .AddProduction("<DivisionAndMultiplicationExpressionTail> => Modulo <ExpressionItem> {ActionOnBinaryModulo} <DivisionAndMultiplicationExpressionTail>")
					   .AddProduction("<DivisionAndMultiplicationExpressionTail> => $")

					   .AddProduction("<ExpressionItem> => LeftParenthesis <Expression> RightParenthesis")
					   .AddProduction("<ExpressionItem> => IntConstant {ActionOnIntConstant}")
					   .AddProduction("<ExpressionItem> => FloatConstant {ActionOnFloatConstant}")
					   .AddProduction("<ExpressionItem> => StringConstant {ActionOnStringConstant}")
					   .AddProduction("<ExpressionItem> => Minus <ExpressionItem> {ActionOnUnaryMinus}")
					   .AddProduction("<ExpressionItem> => Plus <ExpressionItem> {ActionOnUnaryPlus}")
					   .AddProduction("<ExpressionItem> => Negation <ExpressionItem> {ActionOnUnaryNegation}")
					   .AddProduction("<ExpressionItem> => True {ActionOnTrueConstant}")
					   .AddProduction("<ExpressionItem> => False {ActionOnFalseConstant}")
					   .AddProduction("<ExpressionItem> => <Identifier> <ExpressionAfterIdentifier>")
					   .AddProduction("<ExpressionItem> => LeftSquareBracket {ActionOnPrepareArrayLiteralElements} <ArrayExpressions> RightSquareBracket {ActionOnArrayLiteralConstant}")

					   .AddProduction("<ExpressionAfterIdentifier> => LeftParenthesis {ActionOnPrepareFunctionCallArgument} <FunctionCallArguments> RightParenthesis {ActionOnFunctionCallExpression}")
					   .AddProduction("<ExpressionAfterIdentifier> => LeftSquareBracket <Expression> RightSquareBracket {ActionOnAccessingArrayElement} <AdditionalSquareBrackets>")
					   .AddProduction("<ExpressionAfterIdentifier> => $")

					   .AddProduction("<ArrayExpressions> => <ArrayExpression> <ArrayExpressionsTail>")
					   .AddProduction("<ArrayExpressions> => $")

					   .AddProduction("<ArrayExpression> => <Expression> {ActionOnArrayExpression}")

					   .AddProduction("<ArrayExpressionsTail> => Comma <ArrayExpression> <ArrayExpressionsTail>")
					   .AddProduction("<ArrayExpressionsTail> => $")

					   .AddProduction("<Declaration> => <Type> <Identifier> <AssignStatement> Semicolon {ActionOnVariableDeclaration}")

					   .AddProduction("<AssignStatement> => Assign <Expression> {ActionOnAssignStatement}")

					   .AddProduction("<Cycle> => While LeftParenthesis <Expression> RightParenthesis <Statement> {ActionOnWhileStatement}")

					   .AddProduction("<Print> => Print LeftParenthesis {ActionOnPrepareFunctionCallArgument} <FunctionCallArguments> RightParenthesis Semicolon {ActionOnPrintStatement}")

					   .AddProduction("<Read> => Read LeftParenthesis {ActionOnPrepareFunctionCallArgument} <FunctionCallArguments> RightParenthesis Semicolon {ActionOnReadStatement}")

					   .AddProduction("<FunctionCallArguments> => <FunctionCallArgument> <FunctionCallArgumentsTail>")
					   .AddProduction("<FunctionCallArguments> => $")

					   .AddProduction("<FunctionCallArgument> => <Expression> {ActionOnFunctionCallArgument}")

					   .AddProduction("<FunctionCallArgumentsTail> => Comma <FunctionCallArgument> <FunctionCallArgumentsTail>")
					   .AddProduction("<FunctionCallArgumentsTail> => $")

					   .AddProduction("<Return> => Return <ReturnExpression> Semicolon {ActionOnReturnStatement}")

					   .AddProduction("<ReturnExpression> => <Expression> {ActionOnReturnExpression}")
					   .AddProduction("<ReturnExpression> => $")

					   .AddProduction("<CompositeStatement> => LeftCurlyBrace {ActionOnPrepareCompositeStatement} <Statements> RightCurlyBrace {ActionOnCompositeStatement}")

					   .AddProduction("<Statements> => <Statement> {ActionOnCompositeStatementPart} <Statements>")
					   .AddProduction("<Statements> => $")
				
					   .AddProduction(
						   "<StatementAfterIdentifier> => LeftSquareBracket <Expression> RightSquareBracket {ActionOnAccessingArrayElement} <AdditionalSquareBrackets> Assign <Expression> Semicolon {ActionOnAssignStatementArrayElement}"
					   )
					   .AddProduction("<StatementAfterIdentifier> => Assign <Expression> Semicolon {ActionOnAssignStatementAfterIdentifier}")
					   .AddProduction("<StatementAfterIdentifier> => LeftParenthesis {ActionOnPrepareFunctionCallArgument} <FunctionCallArguments> RightParenthesis Semicolon {ActionOnFunctionCallStatement}")

					   .AddProduction("<AdditionalSquareBrackets> => <AdditionalSquareBracket> <AdditionalSquareBrackets>")
					   .AddProduction("<AdditionalSquareBrackets> => $")

					   .AddProduction("<AdditionalSquareBracket> => LeftSquareBracket <Expression> RightSquareBracket {ActionOnAccessingAdditionalSquareBracket}")
					   .Build();

	std::string unmatch;
	if (VerifyGrammarTerminalsMatchLexerTokens(*grammar, unmatch))
	{
		return LLParser(std::make_unique<Lexer>(), CreateParserTable(*grammar), std::cout);
	}
	throw std::logic_error("lexer doesn't know about '" + unmatch + "' token, but grammar does");
}
