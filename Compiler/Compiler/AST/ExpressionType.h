#pragma once

#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

struct ExpressionType
{
	enum class Value
	{
		Void,
		Int,
		Float,
		Bool,
		String,
	};

	Value value;
	size_t nesting = 0;
};

bool operator==(const ExpressionType& left, const ExpressionType& right);
bool operator!=(const ExpressionType& left, const ExpressionType& right);
std::string ToString(const ExpressionType& type);

bool IsConvertible(const ExpressionType& from, const ExpressionType& to);
bool IsConvertibleToBool(const ExpressionType& type);

// Возвращает тип, к которому должна быть приведена левая и правая часть
//  бинарного выражения для выполнения оператора
std::optional<ExpressionType> GetPreferredType(const ExpressionType& left, const ExpressionType& right);
