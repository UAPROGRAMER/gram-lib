#include "lexer.hpp"

// start => 0
// a-z => 1-26
// A-Z => 27-52
// 0-9 => 53-62
// space, newline => 63, 64
// . , : ; - ( ) ! ? & @ ' " => 65-77

void tokenize(std::vector<Token>& tokens, std::string_view source) {
  for (const char a : source) {
    if (a >= 'a' && a <= 'z')
      tokens.push_back(a - 'a' + 1);
    else if (a >= 'A' && a <= 'Z')
      tokens.push_back(a - 'A' + 27);
    else if (a >= '0' && a <= '9')
      tokens.push_back(a - '0' + 53);
    else {
      Token tok;

      switch (a) {
        case ' ':
          tok = 63;
          break;
        case '\n':
          tok = 64;
          break;
        case '.':
          tok = 65;
          break;
        case ',':
          tok = 66;
          break;
        case ':':
          tok = 67;
          break;
        case ';':
          tok = 68;
          break;
        case '-':
          tok = 69;
          break;
        case '(':
          tok = 70;
          break;
        case ')':
          tok = 71;
          break;
        case '!':
          tok = 72;
          break;
        case '?':
          tok = 73;
          break;
        case '&':
          tok = 74;
          break;
        case '@':
          tok = 75;
          break;
        case '\'':
          tok = 76;
          break;
        case '\"':
          tok = 77;
          break;
        default:
          tok = 63;
          break;
      }

      if (tokens.size() > 0 && tokens[tokens.size() - 1] != tok)
        tokens.push_back(tok);
    }
  }
}

char tokenToChar(Token tok) {
  const static char charArr[79] =
      "$"
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789"
      " \n.,:;-()!?&@'\"";

  return tok > 78 ? ' ' : charArr[tok];
}

void tokensWrite(const std::vector<Token>& tokens, std::ostream& stream) {
  for (const auto tok : tokens) stream << tokenToChar(tok);
}