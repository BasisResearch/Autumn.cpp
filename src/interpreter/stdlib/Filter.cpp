
#include "AutumnCallableValue.hpp"
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
Filter::call(Interpreter &interpreter,
             const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 2) {
    if (interpreter.getVerbose()) {
      std::cerr << "Filter() takes 2 arguments, instead got " << arguments.size()
                << std::endl;
    }
    throw Error("Filter() takes 2 arguments");
  }
  std::shared_ptr<AutumnCallableValue> callable =
      std::dynamic_pointer_cast<AutumnCallableValue>(arguments[0]);
  if (callable == nullptr) {
    if (interpreter.getVerbose()) {
      std::cerr << "Filter() first argument must be a callable" << std::endl;
    }
    throw Error("Filter() first argument must be a callable");
  }
  std::shared_ptr<AutumnList> list =
      std::dynamic_pointer_cast<AutumnList>(arguments[1]);
  if (list == nullptr) {
    if (interpreter.getVerbose()) {
      std::cerr << "Filter() second argument must be a list, instead got "
                << arguments[1]->toString() << std::endl;
    }
    throw Error(
        std::string("Filter() second argument must be a list, instead got " +
                    arguments[1]->toString()));
  }
  std::shared_ptr<AutumnList> newList = std::make_shared<AutumnList>();
  for (auto &value : *list->getValues()) {
    std::vector<std::shared_ptr<AutumnValue>> args = {value};
    std::shared_ptr<AutumnValue> result = callable->call(interpreter, args);
    std::shared_ptr<AutumnBool> resultBool =
        std::dynamic_pointer_cast<AutumnBool>(result);
    if (result->isTruthy()) {
      newList->add(value);
    }
  }
  return newList;
}

int Filter::arity() { return 2; }
} // namespace Autumn
