#include <cassert>
#include <iostream>
#include <string>

#include "TokenType.hpp" // Adjust include path to your actual location

// Helper for testing and printing results:
static void testEqual(const std::string &testName, Autumn::TokenType actual,
                      Autumn::TokenType expected) {
  if (actual != expected) {
    std::cerr << "Test Failed: " << testName
              << "\n  Expected: " << Autumn::TokenTypeToString(expected)
              << "\n  Actual:   " << Autumn::TokenTypeToString(actual)
              << std::endl;
    assert(false);
  } else {
    std::cout << "Test Passed: " << testName << std::endl;
  }
}

// Test for getTokenType single-character
void testSingleCharacterTokens() {
  testEqual("LEFT_PAREN", Autumn::getTokenType("("),
            Autumn::TokenType::LEFT_PAREN);
  testEqual("RIGHT_PAREN", Autumn::getTokenType(")"),
            Autumn::TokenType::RIGHT_PAREN);
  testEqual("LEFT_BRACE", Autumn::getTokenType("{"),
            Autumn::TokenType::LEFT_BRACE);
  testEqual("RIGHT_BRACE", Autumn::getTokenType("}"),
            Autumn::TokenType::RIGHT_BRACE);
  testEqual("COMMA", Autumn::getTokenType(","), Autumn::TokenType::COMMA);
  testEqual("DOT", Autumn::getTokenType("."), Autumn::TokenType::DOT);
  testEqual("PLUS", Autumn::getTokenType("+"), Autumn::TokenType::PLUS);
  testEqual("MINUS", Autumn::getTokenType("-"), Autumn::TokenType::MINUS);
  testEqual("STAR", Autumn::getTokenType("*"), Autumn::TokenType::STAR);
  testEqual("SLASH", Autumn::getTokenType("/"), Autumn::TokenType::SLASH);
  testEqual("MODULO", Autumn::getTokenType("%"), Autumn::TokenType::MODULO);
  testEqual("COLON", Autumn::getTokenType(":"), Autumn::TokenType::COLON);
  testEqual("BANG", Autumn::getTokenType("!"), Autumn::TokenType::BANG);
  testEqual("EQUAL", Autumn::getTokenType("="), Autumn::TokenType::EQUAL);
  testEqual("GREATER", Autumn::getTokenType(">"), Autumn::TokenType::GREATER);
  testEqual("LESS", Autumn::getTokenType("<"), Autumn::TokenType::LESS);
}

// Test for getTokenType multi-character
void testMultiCharacterTokens() {
  testEqual("BANG_EQUAL", Autumn::getTokenType("!="),
            Autumn::TokenType::BANG_EQUAL);
  testEqual("EQUAL_EQUAL", Autumn::getTokenType("=="),
            Autumn::TokenType::EQUAL_EQUAL);
  testEqual("GREATER_EQUAL", Autumn::getTokenType(">="),
            Autumn::TokenType::GREATER_EQUAL);
  testEqual("LESS_EQUAL", Autumn::getTokenType("<="),
            Autumn::TokenType::LESS_EQUAL);
  testEqual("MAPTO", Autumn::getTokenType("-->"), Autumn::TokenType::MAPTO);
}

// Test for getTokenType keywords
void testKeywords() {
  testEqual("PROGRAM", Autumn::getTokenType("program"),
            Autumn::TokenType::PROGRAM);
  testEqual("AND", Autumn::getTokenType("and"), Autumn::TokenType::AND);
  testEqual("OBJECT", Autumn::getTokenType("object"),
            Autumn::TokenType::OBJECT);
  testEqual("ELSE", Autumn::getTokenType("else"), Autumn::TokenType::ELSE);
  testEqual("FALSE", Autumn::getTokenType("false"), Autumn::TokenType::FALSE);
  // "fn" in your map is associated with FUN
  testEqual("FUN", Autumn::getTokenType("fn"), Autumn::TokenType::FUN);
  testEqual("IF", Autumn::getTokenType("if"), Autumn::TokenType::IF);
  testEqual("THEN", Autumn::getTokenType("then"), Autumn::TokenType::THEN);
  testEqual("ON", Autumn::getTokenType("on"), Autumn::TokenType::ON);
  testEqual("NIL", Autumn::getTokenType("nil"), Autumn::TokenType::NIL);
  testEqual("OR", Autumn::getTokenType("or"), Autumn::TokenType::OR);
  testEqual("TRUE", Autumn::getTokenType("true"), Autumn::TokenType::TRUE);
  testEqual("LET", Autumn::getTokenType("let"), Autumn::TokenType::LET);
  testEqual("INITNEXT", Autumn::getTokenType("initnext"),
            Autumn::TokenType::INITNEXT);
}

// Test for getTokenType unknown -> IDENTIFIER
void testUnknownIdentifier() {
  testEqual("IDENTIFIER #1", Autumn::getTokenType("someVar"),
            Autumn::TokenType::IDENTIFIER);
  testEqual("IDENTIFIER #2", Autumn::getTokenType("x123"),
            Autumn::TokenType::IDENTIFIER);
  testEqual("IDENTIFIER #3", Autumn::getTokenType(""),
            Autumn::TokenType::IDENTIFIER);
}

// Test for TokenTypeToString correctness
void testTokenTypeToString() {
  // Just check a few samples
  assert(Autumn::TokenTypeToString(Autumn::TokenType::LEFT_PAREN) ==
         "LEFT_PAREN");
  assert(Autumn::TokenTypeToString(Autumn::TokenType::PLUS) == "PLUS");
  assert(Autumn::TokenTypeToString(Autumn::TokenType::MAPTO) == "MAPTO");
  assert(Autumn::TokenTypeToString(Autumn::TokenType::PROGRAM) == "PROGRAM");
  assert(Autumn::TokenTypeToString(Autumn::TokenType::EOF_TOKEN) == "EOF");
  // Check default branch
  // Casting an integer not in the enum to force "UNKNOWN_TOKEN"
  // (This is just an example for thoroughness)
  std::string unknownStr =
      Autumn::TokenTypeToString(static_cast<Autumn::TokenType>(9999));
  assert(unknownStr == "UNKNOWN_TOKEN");

  std::cout << "Test Passed: TokenTypeToString checks" << std::endl;
}

int main() {
  std::cout << "Running TokenType tests...\n";

  testSingleCharacterTokens();
  testMultiCharacterTokens();
  testKeywords();
  testUnknownIdentifier();
  testTokenTypeToString();

  std::cout << "All tests passed successfully.\n";
  return 0;
}
