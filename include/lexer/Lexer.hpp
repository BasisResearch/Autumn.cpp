#ifndef _AUTUMN_LEXER_HPP__
#define _AUTUMN_LEXER_HPP__

#include "Token.hpp"
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

namespace Autumn {
class Lexer {
public:
  Lexer(const std::string &source)
      : source_(source), start_(0), current_(0), line_(1) {}

  std::vector<Token> scanTokens() {
    while (!isAtEnd()) {
      start_ = current_;
      scanToken();
    }

    // Add EOF token at the end
    tokens_.emplace_back(TokenType::EOF_TOKEN, "", "", line_);
    return tokens_;
  }

private:
  const std::string source_;
  size_t start_;
  size_t current_;
  int line_;
  std::vector<Token> tokens_;

  bool isAtEnd() const { return current_ >= source_.length(); }

  char advance() { return source_[current_++]; }

  char peek() const {
    if (isAtEnd())
      return '\0';
    return source_[current_];
  }

  char peekNext() const {
    if (current_ + 1 >= source_.length())
      return '\0';
    return source_[current_ + 1];
  }

  void addToken(TokenType type) {
    std::string text = source_.substr(start_, current_ - start_);
    tokens_.emplace_back(type, text, text, line_);
  }

  void scanToken() {
    char c = advance();
    switch (c) {
    case '(':
      addToken(TokenType::LEFT_PAREN);
      break;
    case ')':
      addToken(TokenType::RIGHT_PAREN);
      break;
    case ',':
      addToken(TokenType::COMMA);
      break;
    case '.':
      if (match('.')) {
        addToken(TokenType::DOTDOT);
      } else
        addToken(TokenType::DOT);
      break;
    case '-':
      if (match('-') && match('>')) {
        addToken(TokenType::MAPTO);
      } else if (match('=')) {
        addToken(TokenType::MINUS); // Adjust if you have '-=' token
      } else {
        addToken(TokenType::MINUS);
      }
      break;
    case '+':
      addToken(TokenType::PLUS);
      break;
    case '/':
      addToken(TokenType::SLASH);
      break;
    case '*':
      addToken(TokenType::STAR);
      break;
    case '&':
      addToken(TokenType::AND);
      break;
    case '|':
      addToken(TokenType::OR);
      break;
    case ':':
      addToken(TokenType::COLON);
      break;
    case '%':
      addToken(TokenType::MODULO);
      break;
    case '!':
      addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      break;
    case '=':
      addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      break;
    case '<':
      addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      break;
    case '>':
      addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    case ' ':
    case '\r':
    case '\t':
      // Ignore whitespace
      break;
    case '\n':
      line_++;
      break;
    case '"':
      string();
      break;
    default:
      if (std::isdigit(c)) {
        number();
      } else if (std::isalpha(c) || c == '_') {
        identifier();
      } else {
        std::cerr << "Unexpected character at line " << line_ << ": '" << c
                  << "'\n";
      }
      break;
    }
  }

  bool match(char expected) {
    if (isAtEnd())
      return false;
    if (source_[current_] != expected)
      return false;
    current_++;
    return true;
  }

  void identifier() {
    while (std::isalnum(peek()) || peek() == '_')
      advance();

    std::string text = source_.substr(start_, current_ - start_);
    TokenType type = getTokenType(text);
    addToken(type);
  }

  void number() {
    while (std::isdigit(peek()))
      advance();

    // Look for a fractional part
    if (peek() == '.' && std::isdigit(peekNext())) {
      // Consume the "."
      advance();

      while (std::isdigit(peek()))
        advance();
    }

    std::string text = source_.substr(start_, current_ - start_);
    addToken(TokenType::NUMBER);
  }

  void string() {
    while (peek() != '"' && !isAtEnd()) {
      if (peek() == '\n')
        line_++;
      advance();
    }

    if (isAtEnd()) {
      std::cerr << "Unterminated string at line " << line_ << "\n";
      return;
    }

    // The closing "
    advance();

    // Trim the surrounding quotes
    std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
    addToken(TokenType::STRING);
  }
};
} // namespace Autumn

#endif
