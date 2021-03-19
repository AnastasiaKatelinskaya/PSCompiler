#include "ProgramAST.h"

void ProgramAST::AddFunction(std::unique_ptr<FunctionAST>&& function)
{
	m_functions.push_back(std::move(function));
}

size_t ProgramAST::GetFunctionsCount() const
{
	return m_functions.size();
}

const FunctionAST& ProgramAST::GetFunction(size_t index) const
{
	if (index >= m_functions.size())
	{
		throw std::out_of_range("index must be less than functions count");
	}
	return *m_functions[index];
}