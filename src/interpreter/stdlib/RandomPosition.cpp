#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

namespace Autumn {
std::shared_ptr<AutumnValue> RandomPositions::call(
    Interpreter &interpreter,
    const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 2) {
    throw Error("randomPositions() takes 2 arguments");
  }
  auto list = std::dynamic_pointer_cast<AutumnList>(arguments[0]);
  auto num = std::dynamic_pointer_cast<AutumnNumber>(arguments[0]);
  if (list == nullptr && num == nullptr) {
    throw Error("RandomPositions() arguments must be a list or a number");
  }
  if (num->getNumber() < 0) {
    throw Error("RandomPositions() argument 1 must be a positive number");
  }

  std::shared_ptr<RandomGenerator> randomGen = interpreter.getRandomGenerator();

  std::shared_ptr<AutumnNumber> arg2 =
      std::dynamic_pointer_cast<AutumnNumber>(arguments[1]);
  if (arg2 == nullptr) {
    throw Error("RandomPositions() argument 2 must be a number");
  }
  std::shared_ptr<AutumnList> newList = std::make_shared<AutumnList>();
  if (list == nullptr) {
    for (int i = 0; i < arg2->getNumber(); i++) {
      newList->add(std::make_shared<AutumnInstance>(
          PositionClass,
          std::vector<std::shared_ptr<AutumnValue>>(
              {std::make_shared<AutumnNumber>(randomGen->next(num->getNumber())),
               std::make_shared<AutumnNumber>(randomGen->next(num->getNumber()))
               })));
    }
  }
  return newList;
}

int RandomPositions::arity() { return 2; }
} // namespace Autumn
