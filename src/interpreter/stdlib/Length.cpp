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
Length::call(Interpreter &interpreter,
             const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 1) {
    if (interpreter.getVerbose()) {
      std::cerr << "Length() takes 1 argument, instead got " << arguments.size()
                << std::endl;
    }
    throw Error("Length() takes 1 argument");
  }
  std::shared_ptr<AutumnList> list =
      std::dynamic_pointer_cast<AutumnList>(arguments[0]);
  if (list == nullptr) {
    if (interpreter.getVerbose()) {
      std::cerr << "Length() argument must be a list, instead got "
                << arguments[0]->toString() << std::endl;
    }
    throw Error("Length() argument must be a list");
  }
  return std::make_shared<AutumnNumber>(list->getValues()->size());
}

} // namespace Autumn
