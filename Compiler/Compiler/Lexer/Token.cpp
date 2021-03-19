#include "Token.h"

std::string TokenToString(const Token& token)
{
	return "Token(" + TokenTypeToString(token.type) + (token.value ? ", " + *token.value : "") + ")";
}
