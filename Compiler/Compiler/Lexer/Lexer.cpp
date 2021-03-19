#include "Lexer.h"

using namespace std::string_literals;

namespace
{
const std::unordered_map<std::string, TokenType> KEYWORDS = {
	{ "FUNCTION", TokenType::Void },
	{ "INTEGER", TokenType::Int },
	{ "FLOAT", TokenType::Float },
	{ "BOOLEAN", TokenType::Bool },
	{ "STRING", TokenType::String },
	{ "ARRAYOF", TokenType::Array },
	{ "IF", TokenType::If },
	{ "WRITE", TokenType::Print },
	{ "READ", TokenType::Read },
	{ "ELSE", TokenType::Else },
	{ "WHILE", TokenType::While },
	{ "RETURN", TokenType::Return },
	{ "TRUE", TokenType::True },
	{ "FALSE", TokenType::False },
	{ "BEGIN", TokenType::LeftCurlyBrace },
	{ "END", TokenType::RightCurlyBrace },
};
}

Lexer::Lexer(const std::string& text)
{
	SetText(text);
}

Token Lexer::GetNextToken()
{
	while (m_ch)
	{
		if (std::isspace(*m_ch))
		{
			SkipWhitespaces();
			continue;
		}
		if (std::isdigit(*m_ch))
		{
			return OnDigit();
		}
		if (std::isalpha(*m_ch) || m_ch == '_')
		{
			return OnAlphaOrUnderscore();
		}
		if (std::ispunct(*m_ch))
		{
			if (m_text.compare(m_pos, 2, "//") == 0)
			{
				SkipUntil('\n');
				Advance();
				continue;
			}
			return OnPunct();
		}

		throw std::runtime_error("can't parse char " + (std::isprint(*m_ch) ? std::to_string(*m_ch) : "#" + std::to_string(int(*m_ch))) + "[" + std::to_string(m_line) + ":" + std::to_string(m_column) + "]");
	}
	return { TokenType::EndOfFile, std::nullopt, m_pos, m_line, m_column };
}

void Lexer::SetText(const std::string& text)
{
	m_text = text;
	m_pos = 0;
	m_column = 0;
	m_line = 0;
	UpdateCh();
}

void Lexer::SkipWhitespaces()
{
	while (m_ch && std::isspace(*m_ch))
	{
		Advance();
	}
}

void Lexer::SkipUntil(char ch)
{
	while (m_ch && m_ch != ch)
	{
		Advance();
	}
}

Token Lexer::OnDigit()
{
	assert(m_ch && std::isdigit(*m_ch));

	std::string value(1, *m_ch);
	Advance();

	while (m_ch && std::isdigit(*m_ch))
	{
		value += *m_ch;
		Advance();
	}

	if (m_ch != '.')
	{
		return { TokenType::IntConstant, value, m_pos, m_line, m_column };
	}

	value += *m_ch;
	Advance();

	while (m_ch && std::isdigit(*m_ch))
	{
		value += *m_ch;
		Advance();
	}
	return { TokenType::FloatConstant, value, m_pos, m_line, m_column };
}

Token Lexer::OnAlphaOrUnderscore()
{
	assert(m_ch && (std::isalpha(*m_ch) || m_ch == '_'));

	std::string value(1, *m_ch);
	Advance();

	while (m_ch && (std::isalnum(*m_ch) || m_ch == '_'))
	{
		value += *m_ch;
		Advance();
	}

	auto found = std::find_if(KEYWORDS.begin(), KEYWORDS.end(), [&value](const auto& pair) {
		const std::string& keyword = pair.first;
		return value == keyword;
	});

	if (found != KEYWORDS.end())
	{
		const TokenType& keyword = found->second;
		return { keyword, std::nullopt, m_pos, m_line, m_column };
	}
	return { TokenType::Identifier, value, m_pos, m_line, m_column };
}

Token Lexer::OnPunct()
{
	assert(m_ch && std::ispunct(*m_ch));

	if (m_text.compare(m_pos, 2, "==") == 0)
	{
		Advance();
		Advance();
		return { TokenType::Equals, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_text.compare(m_pos, 2, "<=") == 0)
	{
		Advance();
		Advance();
		return { TokenType::LessOrEquals, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_text.compare(m_pos, 2, ">=") == 0)
	{
		Advance();
		Advance();
		return { TokenType::MoreOrEquals, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_text.compare(m_pos, 2, "!=") == 0)
	{
		Advance();
		Advance();
		return { TokenType::NotEquals, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_text.compare(m_pos, 2, "->") == 0)
	{
		Advance();
		Advance();
		return { TokenType::Arrow, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_text.compare(m_pos, 2, "||") == 0)
	{
		Advance();
		Advance();
		return { TokenType::Or, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_text.compare(m_pos, 2, "&&") == 0)
	{
		Advance();
		Advance();
		return { TokenType::And, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_text.compare(m_pos, 2, ":=") == 0)
	{
		Advance();
		Advance();
		return { TokenType::Assign, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_text.compare(m_pos, 5, "BEGIN") == 0)
	{
		Advance();
		Advance();
		return { TokenType::LeftCurlyBrace, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_text.compare(m_pos, 3, "END") == 0)
	{
		Advance();
		Advance();
		return { TokenType::RightCurlyBrace, std::nullopt, m_pos, m_line, m_column };
	}
	/*if (m_ch == '=')
	{
		Advance();
		return { TokenType::Assign, std::nullopt, m_pos, m_line, m_column };
	}*/
	if (m_ch == ';')
	{
		Advance();
		return { TokenType::Semicolon, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == ':')
	{
		Advance();
		return { TokenType::Colon, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == ',')
	{
		Advance();
		return { TokenType::Comma, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '(')
	{
		Advance();
		return { TokenType::LeftParenthesis, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == ')')
	{
		Advance();
		return { TokenType::RightParenthesis, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '{')
	{
		Advance();
		return { TokenType::LeftCurlyBrace, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '}')
	{
		Advance();
		return { TokenType::RightCurlyBrace, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '<')
	{
		Advance();
		return { TokenType::LeftAngleBrace, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '>')
	{
		Advance();
		return { TokenType::RightAngleBrace, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '[')
	{
		Advance();
		return { TokenType::LeftSquareBracket, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == ']')
	{
		Advance();
		return { TokenType::RightSquareBracket, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '-')
	{
		Advance();
		return { TokenType::Minus, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '+')
	{
		Advance();
		return { TokenType::Plus, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '*')
	{
		Advance();
		return { TokenType::Multiplication, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '/')
	{
		Advance();
		return { TokenType::Division, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '%')
	{
		Advance();
		return { TokenType::Modulo, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '!')
	{
		Advance();
		return { TokenType::Negation, std::nullopt, m_pos, m_line, m_column };
	}
	if (m_ch == '"')
	{
		const size_t line = m_line;
		const size_t column = m_column;

		Advance();
		std::string value;
		bool escaped = false;

		while (m_ch && m_ch != '\n')
		{
			if (m_ch == '"' && !escaped)
			{
				Advance();
				// replace_all(value, "\\n"s, "\n"s);
				// replace_all(value, "\\t"s, "\t"s);
				return { TokenType::StringConstant, value, m_pos, m_line, m_column };
			}
			escaped = !escaped && m_ch == '\\';
			value += *m_ch;
			Advance();
		}
		throw std::runtime_error("string doesn't have closing quotes on [" + std::to_string(line) + ":" + std::to_string(column) + "]");
	}
	throw std::runtime_error("can't parse punct " + std::string(*m_ch, 1) + " on[" + std::to_string(m_line) + ":" + std::to_string(m_column) + "]");
}

void Lexer::Advance()
{
	if (!m_ch)
	{
		throw std::logic_error("lexer can't advance because end of input has been reached");
	}

	if (m_ch == '\n')
	{
		++m_line;
		m_column = 0;
	}

	++m_pos;
	++m_column;

	UpdateCh();
}

void Lexer::UpdateCh()
{
	m_ch = (m_pos < m_text.length()) ? std::make_optional(m_text[m_pos]) : std::nullopt;
}
