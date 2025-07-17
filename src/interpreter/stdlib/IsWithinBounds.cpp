#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

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

std::shared_ptr<AutumnValue> IsWithinBounds::call(
    Interpreter &interpreter,

    const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  /// TODO: Can break this into two autumn functions
  if (arguments.size() != 1) {
    throw Error("IsWithinBounds() takes 1 argument");
  }
  auto renderedElems = renderValue(interpreter, arguments[0]);
  auto GRID_SIZE = std::dynamic_pointer_cast<AutumnNumber>(
                       interpreter.getEnvironment()->get("GRID_SIZE"))
                       ->getNumber();
  for (auto &elem : *(renderedElems->getValues())) {
    auto instance = std::dynamic_pointer_cast<AutumnInstance>(elem);
    if (instance == nullptr) {
      throw Error("IsWithinBounds() argument must be an instance");
    }
    auto position =
        std::dynamic_pointer_cast<AutumnInstance>(instance->get("position"));
    if (position == nullptr) {
      throw Error("IsWithinBounds() argument must have a position field");
    }
    auto x = std::dynamic_pointer_cast<AutumnNumber>(position->get("x"))
                 ->getNumber();
    auto y = std::dynamic_pointer_cast<AutumnNumber>(position->get("y"))
                 ->getNumber();
    if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE) {
      return std::make_shared<AutumnBool>(false);
    }
  }
  return std::make_shared<AutumnBool>(true);
}

int IsWithinBounds::arity() { return 1; }
} // namespace Autumn
