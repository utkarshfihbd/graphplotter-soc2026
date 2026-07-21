#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include "expr.hpp"
#include <string>

ExprPtr parseExpression(const std::string& text);
ExprPtr loadExpressionFromFile(const std::string& path);

#endif