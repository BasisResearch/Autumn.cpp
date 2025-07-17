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
Range::call(Interpreter &interpreter,
            const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 2) {
    throw Error("Range() takes 2 arguments");
  }
  std::shared_ptr<AutumnNumber> start =
      std::dynamic_pointer_cast<AutumnNumber>(arguments[0]);
  if (start == nullptr) {
    throw Error("Range() first argument must be an integer");
  }
  std::shared_ptr<AutumnNumber> end =
      std::dynamic_pointer_cast<AutumnNumber>(arguments[1]);
  if (end == nullptr) {
    throw Error("Range() second argument must be an integer");
  }
  std::shared_ptr<AutumnList> list = std::make_shared<AutumnList>();
  for (int i = start->getNumber(); i < end->getNumber(); i++) {
    list->add(std::make_shared<AutumnNumber>(i));
  }
  return list;
}
} // namespace Autumn
