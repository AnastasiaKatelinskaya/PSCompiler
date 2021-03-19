#pragma once
#include <stdexcept>
#include <string>
#include <vector>

enum class TokenType
{
	EndOfFile,

	Void,
	Int,
	Float,
	Bool,
	String,
	Array,
	If,
	Else,
	While,
	Return,
	True,
	False,
	Print,
	Read,

	Identifier,
	IntConstant,
	FloatConstant,
	StringConstant,

	Assign,
	LeftParenthesis,
	RightParenthesis,
	LeftAngleBrace,
	RightAngleBrace,
	LeftSquareBracket,
	RightSquareBracket,
	LeftCurlyBrace,
	RightCurlyBrace,
	Arrow,
	Colon,
	Comma,
	Semicolon,

	Plus,
	Minus,
	Multiplication,
	Division,
	Modulo,
	Or,
	And,
	Equals,
	NotEquals,
	LessOrEquals,
	MoreOrEquals,
	Negation
};

bool TokenTypeExists(const std::string& name);
std::string TokenTypeToString(TokenType type);
TokenType StringToTokenType(const std::string& str);
