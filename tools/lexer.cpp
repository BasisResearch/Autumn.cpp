#include "Lexer.hpp"
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  std::vector<std::string> tests = std::vector<std::string>{
      "(object Ant (Cell 0 0 \"gray\"))",
      "(object Light (: on Bool) (Cell 0 0 (if on then \"yellow\" else "
      "\"white\")))",
      "(= lights (initnext (map (--> pos (Light false pos)) (filter (--> "
      "pos (== (.. pos x) (.. pos y))) (allPositions GRID_SIZE))) "
      "(prev lights)))"};

  for (const std::string &test : tests) {
    Autumn::Lexer lexer(test);
    std::vector<Autumn::Token> tokens = lexer.scanTokens();
    std::cout << "Tokens for: " << test << std::endl;
    for (const Autumn::Token &token : tokens) {
      std::cout << token.toString() << std::endl;
    }
  }
  return 0;
}
