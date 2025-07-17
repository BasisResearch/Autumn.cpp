#include "AutumnCallableValue.hpp"
#include "AutumnInstance.hpp"
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
IsList::call(Interpreter &interpreter,
             const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 1) {
    if (interpreter.getVerbose()) {
      std::cerr << "IsList() takes 1 argument, instead got " << arguments.size()
                << std::endl;
    }
    throw Error("IsList() takes 1 argument");
  }
  std::shared_ptr<AutumnValue> value = arguments[0];
  std::shared_ptr<AutumnList> list =
      std::dynamic_pointer_cast<AutumnList>(value);
  if (list == nullptr) {
    return std::make_shared<AutumnBool>(false);
  }
  return std::make_shared<AutumnBool>(true);
}

} // namespace Autumn
