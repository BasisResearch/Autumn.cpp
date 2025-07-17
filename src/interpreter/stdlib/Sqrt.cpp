#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include <Error.hpp>
#include <cmath>
#include <memory>

namespace Autumn {
std::shared_ptr<AutumnValue>
Sqrt::call(Interpreter &interpreter,
           const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 1) {
    throw Error("Sqrt() takes 1 argument");
  }

  std::shared_ptr<AutumnNumber> number =
      std::dynamic_pointer_cast<AutumnNumber>(arguments[0]);
  if (number == nullptr) {
    throw Error("Sqrt() argument must be a number");
  }

  double value = number->getNumber();
  if (value < 0) {
    throw Error("Sqrt() argument must be non-negative");
  }

  return std::make_shared<AutumnNumber>(static_cast<int>(std::sqrt(value)));
}
} // namespace Autumn 