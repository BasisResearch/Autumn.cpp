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

class IdentityFunction : public AutumnCallable {
public:
  IdentityFunction() {}
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override {
    if (arguments.size() != 1) {
      throw Error("Identity function takes exactly 1 argument");
    }
    return arguments[0];
  }
  int arity() override { return 1; }
  std::string toString() const override { return "<native fn: Identity>"; }
  std::shared_ptr<AutumnCallable> clone() override {
    return std::make_shared<IdentityFunction>();
  }
};

std::shared_ptr<AutumnValue>
Any::call(Interpreter &interpreter,
          const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 1 && arguments.size() != 2) {
    if (interpreter.getVerbose()) {
      std::cerr << "Any() takes 1 or 2 arguments, instead got " << arguments.size()
                << std::endl;
    }
    throw Error("Any() takes 1 or 2 arguments");
  }

  std::shared_ptr<AutumnCallableValue> callable;
  std::shared_ptr<AutumnList> list;

  if (arguments.size() == 2) {
    callable = std::dynamic_pointer_cast<AutumnCallableValue>(arguments[0]);
    if (callable == nullptr) {
      if (interpreter.getVerbose()) {
        std::cerr << "Any() first argument must be a callable" << std::endl;
      }
      throw Error("Any() first argument must be a callable");
    }
    list = std::dynamic_pointer_cast<AutumnList>(arguments[1]);
  } else {
    // If only one argument is provided, use the identity function
    callable = std::make_shared<AutumnCallableValue>(std::make_shared<IdentityFunction>());
    list = std::dynamic_pointer_cast<AutumnList>(arguments[0]);
  }

  if (list == nullptr) {
    if (interpreter.getVerbose()) {
      std::cerr << "Any() list argument must be a list, instead got "
                << (arguments.size() == 2 ? arguments[1] : arguments[0])->toString() << std::endl;
    }
    throw Error("Any() list argument must be a list");
  }

  for (auto &value : *list->getValues()) {
    std::vector<std::shared_ptr<AutumnValue>> args = {value};
    std::shared_ptr<AutumnValue> result = callable->call(interpreter, args);
    if (result->isTruthy()) {
      return std::make_shared<AutumnBool>(true);
    }
  }
  return std::make_shared<AutumnBool>(false);
}

} // namespace Autumn
