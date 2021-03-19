#include "TokenType.h"

const std::vector<std::pair<TokenType, std::string>> TOKEN_STRING_BIMAP = {
	{ TokenType::EndOfFile, "EOF" },

	{ TokenType::Void, "Void" },
	{ TokenType::Int, "INTEGER" },
	{ TokenType::Float, "Float" },
	{ TokenType::Bool, "Bool" },
	{ TokenType::Array, "Array" },
	{ TokenType::If, "If" },
	{ TokenType::Else, "Else" },
	{ TokenType::While, "While" },
	{ TokenType::Return, "Return" },
	{ TokenType::True, "True" },
	{ TokenType::False, "False" },
	{ TokenType::String, "String" },
	{ TokenType::Print, "Print" },
	{ TokenType::Read, "Read" },

	{ TokenType::Identifier, "Identifier" },
	{ TokenType::IntConstant, "IntConstant" },
	{ TokenType::FloatConstant, "FloatConstant" },
	{ TokenType::StringConstant, "StringConstant" },

	{ TokenType::Assign, "Assign" },
	{ TokenType::LeftParenthesis, "LeftParenthesis" },
	{ TokenType::RightParenthesis, "RightParenthesis" },
	{ TokenType::LeftAngleBrace, "LeftAngleBrace" },
	{ TokenType::RightAngleBrace, "RightAngleBrace" },
	{ TokenType::LeftSquareBracket, "LeftSquareBracket" },
	{ TokenType::RightSquareBracket, "RightSquareBracket" },
	{ TokenType::LeftCurlyBrace, "LeftCurlyBrace" },
	{ TokenType::RightCurlyBrace, "RightCurlyBrace" },
	{ TokenType::Arrow, "Arrow" },
	{ TokenType::Colon, "Colon" },
	{ TokenType::Comma, "Comma" },
	{ TokenType::Semicolon, "Semicolon" },

	{ TokenType::Minus, "Minus" },
	{ TokenType::Plus, "Plus" },
	{ TokenType::Division, "Division" },
	{ TokenType::Multiplication, "Multiplication" },
	{ TokenType::Modulo, "Modulo" },
	{ TokenType::Or, "Or" },
	{ TokenType::And, "And" },
	{ TokenType::Equals, "Equals" },
	{ TokenType::NotEquals, "NotEquals" },
	{ TokenType::LessOrEquals, "LessOrEquals" },
	{ TokenType::MoreOrEquals, "MoreOrEquals" },
	{ TokenType::Negation, "Negation" }
};

bool TokenTypeExists(const std::string& name)
{
	auto it = std::find_if(TOKEN_STRING_BIMAP.cbegin(), TOKEN_STRING_BIMAP.cend(), [&name](const auto& v) {
		return v.second == name;
	});
	return it != TOKEN_STRING_BIMAP.cend();
}

std::string TokenTypeToString(TokenType type)
{
	auto it = std::find_if(TOKEN_STRING_BIMAP.cbegin(), TOKEN_STRING_BIMAP.cend(), [type](const auto& v) {
		return v.first == type;
	});
	if (it == TOKEN_STRING_BIMAP.cend())
	{
		throw std::logic_error("passed token type doesn't have string representation");
	}
	return it->second;
}

TokenType StringToTokenType(const std::string& str)
{
	auto it = std::find_if(TOKEN_STRING_BIMAP.cbegin(), TOKEN_STRING_BIMAP.cend(), [&str](const auto& v) {
		return v.second == str;
	});
	if (it == TOKEN_STRING_BIMAP.cend())
	{
		throw std::logic_error("passed string doesn't have mapped token type");
	}
	return it->first;
}
