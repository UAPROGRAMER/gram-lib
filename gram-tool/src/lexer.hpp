#pragma once

#include <gram_lib/token.hpp>
#include <ostream>
#include <string_view>
#include <vector>

void tokenize(std::vector<Token>& tokens, std::string_view source);

char tokenToChar(Token tok);

void tokensWrite(const std::vector<Token>& tokens, std::ostream& stream);