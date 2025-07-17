#include "AstPrinter.hpp"
#include "AutumnClass.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::string readFile(const std::string &filename) {
  std::ifstream file(filename);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

int main(int argc, char **argv) {
  std::string mysexpr = readFile("../tests/test2.sexp");
  Autumn::SExpParser parser(mysexpr);
  std::vector<std::shared_ptr<Autumn::Stmt>> stmts = parser.parseStmt();
  Autumn::Interpreter interpreter;
  try {
    interpreter.start(stmts);
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
  }

  std::cout << interpreter.renderAll() << std::endl;
  interpreter.getState()->click(0, 0);
  interpreter.step();
  std::cout << interpreter.renderAll() << std::endl;

  // std::cerr << "Rendered ants: " << renderedAnts->toString() << std::endl;
  // interpreter.step();
  return 0;
}
