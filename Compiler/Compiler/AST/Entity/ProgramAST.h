#pragma once
#include "FunctionAST.h"

#include <stdexcept>

class ProgramAST
{
public:
	void AddFunction(std::unique_ptr<FunctionAST>&& function);

	size_t GetFunctionsCount() const;
	const FunctionAST& GetFunction(size_t index) const;

private:
	std::vector<std::unique_ptr<FunctionAST>> m_functions;
};