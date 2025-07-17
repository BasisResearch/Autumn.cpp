#ifndef AUTUMN_TOKEN_HPP
#define AUTUMN_TOKEN_HPP
#include <TokenType.hpp>
#include <any>
#include <sstream>
#include <string>

namespace Autumn {

class Token {
public:
  // Member variables
  const TokenType type;
  const std::string lexeme;
  const std::any literal;
  const int line; // [location]

  // Constructor
  Token(TokenType type, const std::string &lexeme, const std::any &literal,
        int line)
      : type(type), lexeme(lexeme), literal(literal), line(line) {}

  // toString method
  std::string toString() const;

private:
  // Helper function to convert std::any to string
  std::string literalToString() const;
};

} // namespace Autumn

#endif
