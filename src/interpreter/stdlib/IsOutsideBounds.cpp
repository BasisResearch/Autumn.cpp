#include "AutumnCallableValue.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include "Token.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

namespace Autumn {
std::shared_ptr<AutumnValue> IsOutSideBounds::call(
    Interpreter &interpreter,

    const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  std::shared_ptr<AutumnCallableValue> callable =
      std::dynamic_pointer_cast<AutumnCallableValue>(arguments[0]);
  if (callable == nullptr) {
    throw Error("IsOutSideBounds() first argument must be a callable");
  }
  std::shared_ptr<AutumnBool> isWithinBoundRet =
      std::dynamic_pointer_cast<AutumnBool>(
          callable->call(interpreter, arguments));
  // Invert the result
  return std::make_shared<AutumnBool>(
      !std::any_cast<bool>(isWithinBoundRet->getValue()));
}

int IsOutSideBounds::arity() { return 1; }

} // namespace Autumn
