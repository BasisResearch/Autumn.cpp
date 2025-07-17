#include <Token.hpp>
#include <TokenType.hpp>

namespace Autumn {

std::string Token::literalToString() const {
  if (!literal.has_value()) {
    return "null";
  }

  // Attempt to cast std::any to known types
  if (literal.type() == typeid(double)) {
    return std::to_string(std::any_cast<double>(literal));
  }
  if (literal.type() == typeid(std::string)) {
    return std::any_cast<std::string>(literal);
  }
  if (literal.type() == typeid(bool)) {
    return std::any_cast<bool>(literal) ? "true" : "false";
  }

  // Add more types as needed
  return "unknown";
}

// toString method implementation
std::string Token::toString() const {
  std::stringstream ss;
  ss << TokenTypeToString(type) << " " << lexeme << " " << literalToString();
  return ss.str();
}
} // namespace Autumn
