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
  std::string mysexpr = readFile("../tests/grow.sexp");
  Autumn::SExpParser parser(mysexpr);
  std::vector<std::shared_ptr<Autumn::Stmt>> stmts = parser.parseStmt();
  Autumn::Interpreter interpreter;
  try {
    interpreter.start(stmts);
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
  }

  // LOAD """<filename>"""
  // EVAL
  //
  // CLICK <x> <y>
  // LEFT
  // RIGHT
  // UP
  // DOWN
  //
  // EVAL

  std::cout << interpreter.renderAll() << std::endl;

  interpreter.getState()->pushLeft();
  interpreter.step();
  interpreter.getState()->click(0, 0);
  interpreter.step();
  interpreter.getState()->click(0, 0);
  interpreter.step();
  // std::cout << std::endl << std::endl;
  // std::cout << "Lights after click: " << std::endl;
  // std::cout << interpreter.getEnvironment()->get("lights")->toString()
  //           << std::endl;

  std::cout << interpreter.renderAll() << std::endl;
  // std::cerr << "Rendered ants: " << renderedAnts->toString() << std::endl;
  // interpreter.step();
  return 0;
}
