#ifndef _AUTUMN_ERROR_HPP
#define _AUTUMN_ERROR_HPP

#include "Token.hpp"
#include <exception>
#include <string>

namespace Autumn {
class Error : public std::exception {
  const std::string message;

public:
  Error(const std::string &message) : message(message) {}

  const char *what() const noexcept override { return message.c_str(); }
};

class RuntimeError : public Autumn::Error {
  const std::string message;

public:
  Token token;
  RuntimeError(const Token &token, const std::string &message)
      : Error(message), token(token) {}
  const char *what() const noexcept override { return message.c_str(); }
};
} // namespace Autumn
#endif
