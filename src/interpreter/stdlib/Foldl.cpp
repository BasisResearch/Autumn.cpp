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
std::shared_ptr<AutumnValue>
Foldl::call(Interpreter &interpreter,
            const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 3) {
    throw Error("Foldl() takes 3 arguments");
  }
  std::shared_ptr<AutumnCallableValue> callable =
      std::dynamic_pointer_cast<AutumnCallableValue>(arguments[0]);
  if (callable == nullptr) {
    throw Error("Foldl() first argument must be a callable");
  }
  std::shared_ptr<AutumnValue> acc = arguments[1];
  std::shared_ptr<AutumnList> list =
      std::dynamic_pointer_cast<AutumnList>(arguments[2]);
  if (list == nullptr) {
    throw Error("Foldl() third argument must be a list");
  }
  for (auto &value : *list->getValues()) {
    std::vector<std::shared_ptr<AutumnValue>> args = {acc, value};
    acc = callable->call(interpreter, args);
  }
  return acc;
}

} // namespace Autumn
