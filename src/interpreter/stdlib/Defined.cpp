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
Defined::call(Interpreter &Interpreter,
              const std::vector<std::shared_ptr<AutumnValue>> &Arguments) {
  if (Arguments.size() != 1) {
    throw Error("Defined() takes 1 argument");
  }
  // Only store the global variable
  std::shared_ptr<AutumnString> name =
      std::dynamic_pointer_cast<AutumnString>(Arguments[0]);
  if (name != nullptr) {
    // if name starts with \" then clip it
    std::string varName = name->getString();
    if (varName[0] == '\"') {
      varName = varName.substr(1, varName.size() - 2);
    }
    auto defineds = Interpreter.getGlobals()->getDefinedVariables();
    if (defineds.find(varName) != defineds.end()) {
      return std::make_shared<AutumnBool>(true);
    }
    return std::make_shared<AutumnBool>(false);
  }
  throw Error("Defined() argument must be a string");
}

} // namespace Autumn
