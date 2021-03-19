#include "GrammarSymbol.h"

GrammarSymbol::GrammarSymbol(std::string text, GrammarSymbolType type, std::optional<std::string> attribute /* = none */)
	: m_text(std::move(text))
	, m_type(type)
	, m_attribute(std::move(attribute))
{
}

const std::string& GrammarSymbol::GetText() const
{
	return m_text;
}

GrammarSymbolType GrammarSymbol::GetType() const
{
	return m_type;
}

void GrammarSymbol::SetAttribute(std::string attribute)
{
	m_attribute = std::move(attribute);
}

std::optional<std::string> GrammarSymbol::GetAttribute() const
{
	return m_attribute;
}
