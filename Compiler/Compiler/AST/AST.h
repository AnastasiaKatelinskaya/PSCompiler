#pragma once

#include "Entity/BuiltinCallStatementAST.h"
#include "Entity/BinaryExpressionAST.h"
#include "Entity/UnaryAST.h"

#include <stdexcept>

std::string ToString(BinaryExpressionAST::Operator operation);
std::string ToString(UnaryAST::Operator operation);
std::string ToString(BuiltinCallStatementAST::Builtin builtIn);
