#pragma once

#include "../Lexer/Lexer.h"
#include "../Parser/LLParser.h"
#include "../Parser/LLParserTable.h"
#include "../Utils/file_utils.h"
#include "../grammarlib/Grammar.h"
#include "../grammarlib/GrammarBuilder.h"
#include "../grammarlib/GrammarProductionFactory.h"

#include <iostream>

class CompilerDriver
{
public:
	explicit CompilerDriver(std::ostream& log);

	void Compile(const std::string& text);

private:
	bool VerifyGrammarTerminalsMatchLexerTokens(const Grammar& grammar, std::string& unmatch);
	LLParser CreateParser();

	std::ostream& m_log;
};
