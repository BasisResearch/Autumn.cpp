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
  std::string mysexpr = readFile(argv[1]);
  Autumn::SExpParser parser(mysexpr);
  std::vector<std::shared_ptr<Autumn::Stmt>> stmts = parser.parseStmt();
  Autumn::Interpreter interpreter;
  try {
    interpreter.start(stmts);
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
  }
  interpreter.getEnvironment()
      ->get(Autumn::Token(Autumn::TokenType::IDENTIFIER, "ants", "ants", -1))
      ->toString();
  std::cout << std::endl << std::endl;

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

  try {
    std::string getRenderedAnts =
        "concat (map (--> a ((.. a \"render\"))) ants)";
    auto sexp = sexpresso::parse(getRenderedAnts);
    Autumn::SExpParser renderParser(getRenderedAnts);
    std::shared_ptr<Autumn::Expr> renderExpr = renderParser.parseExpr(sexp);
    Autumn::AstPrinter printer;
    std::cerr << "Render expr: "
              << std::any_cast<std::string>(renderExpr->accept(printer))
              << std::endl;
    std::shared_ptr<Autumn::AutumnValue> renderedAnts =
        std::any_cast<std::shared_ptr<Autumn::AutumnValue>>(
            renderExpr->accept(interpreter));

    std::cout << std::endl << std::endl;
    std::cout << "Rendered ants: " << std::endl;
    std::cout << renderedAnts->toString() << std::endl;
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
  }

  // std::cerr << "Rendered ants: " << renderedAnts->toString() << std::endl;
  // interpreter.step();
  return 0;
}
