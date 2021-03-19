#pragma once

#include "GrammarProduction.h"
#include "GrammarSymbol.h"

#include <cassert>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>

// Reserved strings for special symbols
const std::string ARROW_SYMBOL = "=>";
const std::string EPSILON_SYMBOL = "$";

class GrammarProductionFactory
{
public:
	std::shared_ptr<GrammarProduction> CreateProduction(const std::string& line);

private:
	bool MatchSafely(const std::string& str, size_t& offset, const std::string& match);
	void SkipWhile(const std::string& str, size_t& offset, std::function<bool(char ch)>&& predicate);
	void ReadWhile(const std::string& text, std::string& characters, size_t& offset, std::function<bool(char ch)>&& predicate);

	bool ReadAs(const std::string& text, size_t& offset, const std::string& what);
	bool ReadChars(const std::string& text, size_t& offset, std::string& chars);
	bool ReadCharsEx(const std::string& text, size_t& offset, std::string& chars, const std::string& prefix, const std::string& suffix);
	bool ReadAsArrow(const std::string& text, size_t& offset);
	bool ReadAsEpsilon(const std::string& text, size_t& offset);
	bool ReadAsTerminal(const std::string& text, size_t& offset, std::string& terminal);
	bool ReadAsNonterminal(const std::string& text, size_t& offset, std::string& nonterminal);
	bool ReadAsAttribute(const std::string& text, size_t& offset, std::string& attribute);
};
