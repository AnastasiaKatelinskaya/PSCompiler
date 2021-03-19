#pragma once
#include "../AST/AST.h"
#include "../AST/ASTBuilder.h"
#include "../AST/Entity/ProgramAST.h"
#include "../Lexer/ILexer.h"
#include "IParser.h"
#include "LLParserTable.h"

#include <cassert>
#include <functional>
#include <ostream>
#include <unordered_map>

class LLParser : public IParser<ProgramAST>
{
public:
	explicit LLParser(
		std::unique_ptr<ILexer>&& lexer,
		std::unique_ptr<LLParserTable>&& table,
		std::ostream& output);

	ProgramAST Parse(const std::string& text) override;

private:
	std::unique_ptr<ILexer> m_lexer;
	std::unique_ptr<LLParserTable> m_table;
	std::ostream& m_output;
};
