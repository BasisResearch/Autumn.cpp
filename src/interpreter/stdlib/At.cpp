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
At::call(Interpreter &interpreter,
           const std::vector<std::shared_ptr<AutumnValue>> &arguments) {

  if (arguments.size() != 2) {
    throw Error("At() takes 2 arguments");
  }
  std::shared_ptr<AutumnList> list =
      std::dynamic_pointer_cast<AutumnList>(arguments[0]);
  if (list == nullptr) {
    throw Error("At() first argument must be a list");
  }
  if (list->getValues()->size() == 0) {
    throw Error("At() first argument must not be an empty list");
  }
  std::shared_ptr<AutumnNumber> index =
      std::dynamic_pointer_cast<AutumnNumber>(arguments[1]);
  if (index == nullptr) {
    throw Error("At() second argument must be a number");
  }
  int indexValue = index->getNumber();
  if (indexValue < 0 || indexValue >= list->getValues()->size()) {
    throw Error("At() index out of bounds");
  }
  return list->getValues()->at(indexValue);
}
} // namespace Autumn
