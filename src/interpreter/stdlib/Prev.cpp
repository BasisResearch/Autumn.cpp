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
Prev::call(Interpreter &Interpreter,
           const std::vector<std::shared_ptr<AutumnValue>> &Arguments) {
  if (Arguments.size() != 1) {
    throw Error("[Prev] takes 1 argument, instead got " + std::to_string(Arguments.size()));
  }
  // Only store the global variable
  std::shared_ptr<Environment> prev_environment = Interpreter.getPrevEnvironment();
  if (prev_environment == nullptr) {
    throw Error("[Prev] Prev environment not initialized");
  }
  std::shared_ptr<AutumnString> name =
      std::dynamic_pointer_cast<AutumnString>(Arguments[0]);
  if (name != nullptr) {
    // if name starts with \" then clip it
    std::string varName = name->getString();
    if (varName[0] == '\"') {
      varName = varName.substr(1, varName.size() - 2);
    }
    try{
      std::shared_ptr<AutumnValue> value = prev_environment->get(varName);
      return value;
    } catch (Error &e) {
      throw Error("[Prev] Not found: " +
                  name->getString() + ", all defined variables: " +
                  prev_environment->toJson() + "\n");
    }
  }
  int instId = Arguments[0]->getInstId();
  std::shared_ptr<AutumnValue> value = prev_environment->findId(instId);
  if (value == nullptr) {
    throw Error("[Prev] Not found: " +
                std::to_string(instId) + ", all defined variables: " +
                prev_environment->toJson() + "\n");
  }
  return value;
}

int Prev::arity() { return 1; }
} // namespace Autumn
