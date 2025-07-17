#include "AutumnCallableValue.hpp"
#include "AutumnInstance.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include "sexpresso.hpp"
#include <Error.hpp>
#include <any>
#include <cassert>
#include <memory>
#include <string>

namespace Autumn {

static std::shared_ptr<AutumnList>
renderValue(Interpreter &interpreter, std::shared_ptr<AutumnValue> value) {
  auto pList = std::dynamic_pointer_cast<AutumnList>(value);
  auto pInstance = std::dynamic_pointer_cast<AutumnInstance>(value);
  if (pList != nullptr) {
    std::vector<std::shared_ptr<AutumnValue>> values;
    values.reserve(pList->getValues()->size() * 3);
    for (auto &elem : *(pList->getValues())) {
      auto pRendered = renderValue(interpreter, elem);
      values.insert(values.end(), pRendered->getValues()->begin(),
                    pRendered->getValues()->end());
    }
    return std::make_shared<AutumnList>(
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(values));
  } else if (pInstance != nullptr) {
    if (pInstance->getClass()->findMethod("render") == nullptr) {
      if (interpreter.getVerbose()) {
        std::cerr << "Warning: Instance does not have render method" << std::endl;
      }
      return std::make_shared<AutumnList>(
          std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(
              std::vector<std::shared_ptr<AutumnValue>>()));
    }
    std::shared_ptr<AutumnCallableValue> render =
        std::dynamic_pointer_cast<AutumnCallableValue>(
            pInstance->get("render"));
    std::shared_ptr<AutumnValue> result = render->call(interpreter, {});
    return std::dynamic_pointer_cast<AutumnList>(result);
  } else {
    return std::make_shared<AutumnList>(
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(
            std::vector<std::shared_ptr<AutumnValue>>({})));
  }
}

std::shared_ptr<AutumnValue>
Clicked::call(Interpreter &interpreter,
              const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (interpreter.getState()->getClicked() == false) {
    return std::make_shared<AutumnBool>(false);
  }
  interpreter.getGlobals()->define(
      "click",
      std::make_shared<AutumnInstance>(
          PositionClass,
          std::vector<std::shared_ptr<AutumnValue>>(
              {std::make_shared<AutumnNumber>(interpreter.getState()->getX()),
               std::make_shared<AutumnNumber>(
                   interpreter.getState()->getY())})));
  if (arguments.size() == 0) {
    return std::make_shared<AutumnBool>(true);
  }
  if (arguments.size() == 1) {
    // First, render the obj
    auto renderedElem = renderValue(interpreter, arguments[0]);
    // Filter out the element that is clicked
    std::string positionString =
        std::string("Position ") +
        std::to_string(interpreter.getState()->getX()) + " " +
        std::to_string(interpreter.getState()->getY());
    // std::cout << "Clicked at " << positionString << std::endl;
    // std::cout << "Object to be tested: " << renderedElem->toString()
    //           << std::endl;
    for (auto &elem : *(renderedElem->getValues())) {
      auto elemInstance = std::dynamic_pointer_cast<AutumnInstance>(elem);
      if (elemInstance == nullptr) {
        continue;
      }
      if (elemInstance->get("position") == nullptr) {
        continue;
      }
      auto position = std::dynamic_pointer_cast<AutumnInstance>(
          elemInstance->get("position"));
      if (position == nullptr) {
        continue;
      }
      if (position->get("x") == nullptr || position->get("y") == nullptr) {
        continue;
      }
      auto x = std::dynamic_pointer_cast<AutumnNumber>(position->get("x"))
                   ->getNumber();
      auto y = std::dynamic_pointer_cast<AutumnNumber>(position->get("y"))
                   ->getNumber();
      if (x == interpreter.getState()->getX() &&
          y == interpreter.getState()->getY()) {
        return std::make_shared<AutumnBool>(true);
      }
    }
    return std::make_shared<AutumnBool>(false);
  } else {
    throw Error("Clicked() takes 0..1 argument(s), got " +
                std::to_string(arguments.size()));
  }
};

int Clicked::arity() { return 2; }
} // namespace Autumn
