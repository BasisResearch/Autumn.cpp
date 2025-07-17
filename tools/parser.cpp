#include "sexpresso.hpp"
// adding file handling
#include "AstPrinter.hpp"
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
  Autumn::AstPrinter printer;
  try {
    std::vector<std::shared_ptr<Autumn::Stmt>> stmts = parser.parseStmt();
    for (const auto &stmt : stmts) {
      std::cout << printer.print(stmt) << std::endl;
    }
  } catch (const std::runtime_error &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
