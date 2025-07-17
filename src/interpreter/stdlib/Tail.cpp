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
Tail::call(Interpreter &interpreter,
           const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 1) {
    throw Error("Tail() takes 1 argument");
  }
  std::shared_ptr<AutumnList> list =
      std::dynamic_pointer_cast<AutumnList>(arguments[0]);
  if (list == nullptr) {
    throw Error("Tail() argument must be a list");
  }
  if (list->getValues()->size() == 0) {
    throw Error("Tail() argument must not be an empty list");
  }
  std::shared_ptr<AutumnValue> tail =
      list->getValues()->at(list->getValues()->size() - 1);
  return tail;
}
} // namespace Autumn
