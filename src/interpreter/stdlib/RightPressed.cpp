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
RightPressed::call(Interpreter &interpreter,
                   const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  return std::make_shared<AutumnBool>(interpreter.getState()->getRight());
};

int RightPressed::arity() { return 2; }
} // namespace Autumn
