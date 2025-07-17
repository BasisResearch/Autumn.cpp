#ifndef AUTUMN_TOKEN_TYPE_HPP
#define AUTUMN_TOKEN_TYPE_HPP

#include <string>
#include <unordered_map>
namespace Autumn {
enum class TokenType {
  // Single-character tokens.
  LEFT_PAREN,  // '('
  RIGHT_PAREN, // ')'
  LEFT_BRACE,  // '{'
  RIGHT_BRACE, // '}'
  COMMA,       // ','
  DOT,         // '.'
  DOTDOT,      // '.'
  MINUS,       // '-'
  PLUS,        // '+'
  SLASH,       // '/'
  STAR,        // '*',
  COLON,       // ':'
  MODULO,      // '%'

  // One or two character tokens.
  BANG,          // '!'
  BANG_EQUAL,    // '!='
  EQUAL,         // '='
  EQUAL_EQUAL,   // '=='
  GREATER,       // '>'
  GREATER_EQUAL, // '>='
  LESS,          // '<'
  LESS_EQUAL,    // '<=',
  MAPTO,         // -->

  // Literals.
  IDENTIFIER, // e.g., variable names
  STRING,     // e.g., "hello"
  NUMBER,     // e.g., 123, 45.67

  // Keywords.
  PROGRAM, // 'program'
  AND,     // 'and'
  OBJECT,  // 'object'
  ELSE,    // 'else'
  FALSE,   // 'false'
  FUN,     // 'fun'
  IF,      // 'if'
  THEN,    // 'then'
  ON,
  NIL,      // 'nil'
  OR,       // 'or'
  TRUE,     // 'true'
  INITNEXT, // 'var'
  LET,      // 'let'

  EOF_TOKEN // End-of-file marker
};

inline std::string TokenTypeToString(TokenType type) {
  switch (type) {
  // Single-character tokens.
  case TokenType::LEFT_PAREN:
    return "LEFT_PAREN";
  case TokenType::RIGHT_PAREN:
    return "RIGHT_PAREN";
  case TokenType::LEFT_BRACE:
    return "LEFT_BRACE";
  case TokenType::RIGHT_BRACE:
    return "RIGHT_BRACE";
  case TokenType::COMMA:
    return "COMMA";
  case TokenType::DOT:
    return "DOT";
  case TokenType::DOTDOT:
    return "DOTDOT";
  case TokenType::MINUS:
    return "MINUS";
  case TokenType::PLUS:
    return "PLUS";
  case TokenType::SLASH:
    return "SLASH";
  case TokenType::STAR:
    return "STAR";
  case TokenType::COLON:
    return "COLON";
  case TokenType::MODULO:
    return "MODULO";
  case TokenType::MAPTO:
    return "MAPTO";

  // One or two character tokens.
  case TokenType::BANG:
    return "BANG";
  case TokenType::BANG_EQUAL:
    return "BANG_EQUAL";
  case TokenType::EQUAL:
    return "EQUAL";
  case TokenType::EQUAL_EQUAL:
    return "EQUAL_EQUAL";
  case TokenType::GREATER:
    return "GREATER";
  case TokenType::GREATER_EQUAL:
    return "GREATER_EQUAL";
  case TokenType::LESS:
    return "LESS";
  case TokenType::LESS_EQUAL:
    return "LESS_EQUAL";

  // Literals.
  case TokenType::IDENTIFIER:
    return "IDENTIFIER";
  case TokenType::STRING:
    return "STRING";
  case TokenType::NUMBER:
    return "NUMBER";

  // Keywords.
  case TokenType::PROGRAM:
    return "PROGRAM";
  case TokenType::AND:
    return "AND";
  case TokenType::OBJECT:
    return "OBJECT";
  case TokenType::ELSE:
    return "ELSE";
  case TokenType::FALSE:
    return "FALSE";
  case TokenType::FUN:
    return "FUN";
  case TokenType::IF:
    return "IF";
  case TokenType::NIL:
    return "NIL";
  case TokenType::OR:
    return "OR";
  case TokenType::ON:
    return "ON";
  case TokenType::THEN:
    return "THEN";
  case TokenType::TRUE:
    return "TRUE";
  case TokenType::LET:
    return "LET";
  case TokenType::INITNEXT:
    return "INITNEXT";

  case TokenType::EOF_TOKEN:
    return "EOF";
  default:
    return "UNKNOWN_TOKEN";
  }
}

inline TokenType getTokenType(const std::string &token) {
  // Define keyword map
  static const std::unordered_map<std::string, TokenType> keywords = {
      {"program", TokenType::PROGRAM}, {"and", TokenType::AND},
      {"object", TokenType::OBJECT},   {"else", TokenType::ELSE},
      {"false", TokenType::FALSE},     {"fn", TokenType::FUN},
      {"if", TokenType::IF},           {"then", TokenType::THEN},
      {"nil", TokenType::NIL},         {"or", TokenType::OR},
      {"true", TokenType::TRUE},       {"on", TokenType::ON},
      {"let", TokenType::LET},         {"initnext", TokenType::INITNEXT}};

  // Define multi-character symbols map
  static const std::unordered_map<std::string, TokenType> multiCharSymbols = {
      {"!=", TokenType::BANG_EQUAL},
      {"==", TokenType::EQUAL_EQUAL},
      {">=", TokenType::GREATER_EQUAL},
      {"<=", TokenType::LESS_EQUAL},
      {"-->", TokenType::MAPTO}};

  // Define single-character symbols map
  static const std::unordered_map<char, TokenType> singleCharSymbols = {
      {'(', TokenType::LEFT_PAREN}, {')', TokenType::RIGHT_PAREN},
      {'{', TokenType::LEFT_BRACE}, {'}', TokenType::RIGHT_BRACE},
      {',', TokenType::COMMA},      {'.', TokenType::DOT},
      {'-', TokenType::MINUS},      {'+', TokenType::PLUS},
      {'/', TokenType::SLASH},      {'*', TokenType::STAR},
      {'%', TokenType::MODULO},     {':', TokenType::COLON},
      {'!', TokenType::BANG},       {'=', TokenType::EQUAL},
      {'>', TokenType::GREATER},    {'<', TokenType::LESS},
      {'&', TokenType::AND},        {'|', TokenType::OR}};

  // First, check if the token matches any multi-character symbols
  auto it = multiCharSymbols.find(token);
  if (it != multiCharSymbols.end()) {
    return it->second;
  }

  // If the token is a single character, check the singleCharSymbols map
  if (token.length() == 1) {
    char c = token[0];
    auto it_single = singleCharSymbols.find(c);
    if (it_single != singleCharSymbols.end()) {
      return it_single->second;
    }
  }

  // Check if the token is a keyword
  auto it_keyword = keywords.find(token);
  if (it_keyword != keywords.end()) {
    return it_keyword->second;
  }

  // If the token does not match any known symbol or keyword, it's an IDENTIFIER
  return TokenType::IDENTIFIER;
}

} // namespace Autumn

#endif
