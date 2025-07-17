#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

namespace Autumn {
std::shared_ptr<AutumnValue>
AllPositions::call(Interpreter &interpreter,
                   const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() > 2 || arguments.size() < 1) {
    // std::cerr << "AllPositions() takes at minimum 0 argument and at most 2 "
    //              "arguments"
    //           << std::endl;
    throw Error(
        "AllPositions() takes at minimum 1 argument and at most 2 arguments");
  }
  if (arguments.size() == 1) {
    std::shared_ptr<AutumnNumber> num =
        std::dynamic_pointer_cast<AutumnNumber>(arguments[0]);
    if (num == nullptr) {
      throw Error("AllPositions() argument 1 must be a number");
    }
    if (num->getNumber() < 0) {
      throw Error("AllPositions() argument 1 must be a positive number");
    }
    std::shared_ptr<AutumnList> newList = std::make_shared<AutumnList>();
    for (int i = 0; i < num->getNumber(); i++) {
      for (int j = 0; j < num->getNumber(); j++) {
        newList->add(std::make_shared<AutumnInstance>(
            PositionClass, std::vector<std::shared_ptr<AutumnValue>>(
                               {std::make_shared<AutumnNumber>(i),
                                std::make_shared<AutumnNumber>(j)})));
      }
    }
    return newList;
  } else {
    std::shared_ptr<AutumnNumber> num1 =
        std::dynamic_pointer_cast<AutumnNumber>(arguments[0]);
    if (num1 == nullptr) {
      // std::cerr << "AllPositions() argument 1 must be a number" << std::endl;
      throw Error("AllPositions() argument 1 must be a number");
    }
    if (num1->getNumber() < 0) {
      // std::cerr << "AllPositions() argument 1 must be a positive number"
      // << std::endl;
      throw Error("AllPositions() argument 1 must be a positive number");
    }
    std::shared_ptr<AutumnNumber> num2 =
        std::dynamic_pointer_cast<AutumnNumber>(arguments[1]);
    if (num2 == nullptr) {
      // std::cerr << "AllPositions() argument 2 must be a number" << std::endl;
      throw Error("AllPositions() argument 2 must be a number");
    }
    if (num2->getNumber() < 0) {
      // std::cerr << "AllPositions() argument 2 must be a positive number"
      //           << std::endl;
      throw Error("AllPositions() argument 2 must be a positive number");
    }
    std::shared_ptr<AutumnList> newList = std::make_shared<AutumnList>();
    for (int i = 0; i < num1->getNumber(); i++) {
      for (int j = 0; j < num2->getNumber(); j++) {
        newList->add(std::make_shared<AutumnInstance>(
            PositionClass, std::vector<std::shared_ptr<AutumnValue>>(
                               {std::make_shared<AutumnNumber>(i),
                                std::make_shared<AutumnNumber>(j)})));
      }
    }
    return newList;
  }
}

int AllPositions::arity() { return 2; }
} // namespace Autumn
