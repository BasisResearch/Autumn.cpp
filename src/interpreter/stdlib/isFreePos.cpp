#include "AutumnCallableValue.hpp"
#include "AutumnInstance.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include "sexpresso.hpp"
#include <Error.hpp>
#include <any>
#include <cassert>
#include <memory>
#include <string>

namespace Autumn {
std::shared_ptr<AutumnValue>
IsFreePos::call(Interpreter &interpreter,
                const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 1) {
    throw Error("isFreePos requires 1 argument");
  }
  auto pInstance = std::dynamic_pointer_cast<AutumnInstance>(arguments[0]);
  if (pInstance == nullptr) {
    throw Error("isFreePos requires an instance as argument");
  }
  if (pInstance->getClass() != PositionClass) {
    throw Error("isFreePos requires a Position instance as argument");
  }
  int x = std::any_cast<int>(pInstance->get("x")->getValue());
  int y = std::any_cast<int>(pInstance->get("y")->getValue());
  return std::make_shared<AutumnBool>(
      interpreter.getGlobals()->isFreePos(x, y));
}
} // namespace Autumn
