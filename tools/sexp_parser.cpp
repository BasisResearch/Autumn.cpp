#include "sexpresso.hpp"
// adding file handling
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
  std::string mysexpr = readFile("tests/test.sexp");
  auto parsetree = sexpresso::parse(mysexpr);
  // Print head
  std::cout << "Head: " << parsetree->getChild(0)->getChild(0)->toString()
            << std::endl;
  std::cout << "Head is string: "
            << parsetree->getChild(0)->getChild(0)->isString() << std::endl;

  // Print tail
  std::cout << "1st arg " << parsetree->getChild(0)->getChild(1)->toString()
            << std::endl;
  std::cout << "1st arg is string: "
            << parsetree->getChild(0)->getChild(1)->isString() << std::endl;
  // Print tail's Head
  std::cout << "2nd Head: " << parsetree->getChild(0)->getChild(2)->toString()
            << std::endl;
  return 0;
}
