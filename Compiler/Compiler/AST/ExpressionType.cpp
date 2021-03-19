#include "ExpressionType.h"

namespace
{
const std::unordered_map<ExpressionType::Value, std::unordered_set<ExpressionType::Value>> gcAvailableCasts = {
	{ ExpressionType::Value::Int, { ExpressionType::Value::Float, ExpressionType::Value::Bool } },
	{ ExpressionType::Value::Float, { ExpressionType::Value::Int, ExpressionType::Value::Bool } },
	{ ExpressionType::Value::Bool, { ExpressionType::Value::Int, ExpressionType::Value::Float } }
};

const std::map<std::pair<ExpressionType::Value, ExpressionType::Value>, ExpressionType::Value> gcBinaryCasts = {
	{ { ExpressionType::Value::Int, ExpressionType::Value::Float }, ExpressionType::Value::Float },
	{ { ExpressionType::Value::Int, ExpressionType::Value::Bool }, ExpressionType::Value::Int },
	{ { ExpressionType::Value::Float, ExpressionType::Value::Int }, ExpressionType::Value::Float },
	{ { ExpressionType::Value::Float, ExpressionType::Value::Bool }, ExpressionType::Value::Float },
	{ { ExpressionType::Value::Bool, ExpressionType::Value::Int }, ExpressionType::Value::Int },
	{ { ExpressionType::Value::Bool, ExpressionType::Value::Float }, ExpressionType::Value::Float }
};
}

bool operator ==(const ExpressionType& left, const ExpressionType& right)
{
	return left.value == right.value && left.nesting == right.nesting;
}

bool operator !=(const ExpressionType& left, const ExpressionType& right)
{
	return !(left == right);
}

bool IsConvertible(const ExpressionType& from, const ExpressionType& to)
{
	if (from.nesting != 0 || to.nesting != 0)
	{
		throw std::runtime_error("array conversion is not supported");
	}

	if (from == to)
	{
		throw std::runtime_error("trying to convert from '" + ToString(from) +  "' to itself");
	}

	auto found = gcAvailableCasts.find(from.value);
	if (found == gcAvailableCasts.end())
	{
		return false;
	}

	const std::unordered_set<ExpressionType::Value>& availableCasts = found->second;
	return availableCasts.find(to.value) != availableCasts.end();
}

bool IsConvertibleToBool(const ExpressionType& type)
{
	return IsConvertible(type, { ExpressionType::Value::Bool, 0 });
}

std::optional<ExpressionType> GetPreferredType(const ExpressionType& left, const ExpressionType& right)
{
	if (left.nesting != 0 || right.nesting != 0)
	{
		return std::nullopt;
	}

	if (left == right)
	{
		return left;
	}

	auto found = gcBinaryCasts.find(std::make_pair(left.value, right.value));
	if (found == gcBinaryCasts.end())
	{
		return std::nullopt;
	}
	return ExpressionType{ found->second, 0 };
}

std::string ToString(const ExpressionType& type)
{
	// Type can represent array
	if (type.nesting != 0)
	{
		const ExpressionType stored = { type.value, type.nesting - 1 };
		return "array<" + ToString(stored) + ">";
	}

	switch (type.value)
	{
	case ExpressionType::Value::Void:
		return "void";
	case ExpressionType::Value::Int:
		return "int";
	case ExpressionType::Value::Float:
		return "float";
	case ExpressionType::Value::Bool:
		return "bool";
	case ExpressionType::Value::String:
		return "string";
	default:
		throw std::logic_error("can't get string representation of undefined expression type");
	}
}
