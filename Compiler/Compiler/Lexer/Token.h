#pragma once
#include "TokenType.h"

#include <optional>

struct Token
{
	TokenType type = TokenType::EndOfFile;
	std::optional<std::string> value = std::nullopt;
	size_t offset = 0;
	size_t line = 0;
	size_t column = 0;
};

std::string TokenToString(const Token& token);
