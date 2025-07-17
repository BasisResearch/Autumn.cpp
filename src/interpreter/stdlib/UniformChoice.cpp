// uniformChoice.cpp
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Interpreter.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

namespace Autumn {

std::shared_ptr<AutumnValue> UniformChoice::call(
    Interpreter &interpreter,
    const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  // Check if at least one argument is provided
  if (arguments.empty()) {
    throw Error("uniformChoice() requires at least 1 argument.");
  }

  // Extract freePositions from arguments[0]
  // Assuming arguments[0] is an AutumnList
  auto freeValueList = std::dynamic_pointer_cast<AutumnList>(arguments[0]);
  if (!freeValueList) {
    throw Error("uniformChoice() argument 1 must be a list of values.");
  }
  std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>> pValueList =
      freeValueList->getValues();
  std::vector<std::shared_ptr<AutumnValue>> &valueList = *pValueList;

  size_t freeSize = valueList.size();
  if (freeSize == 0) {
    throw Error("uniformChoice() received an empty list of free positions.");
  }

  if (arguments.size() == 1) {
    // Return a single random element
    std::shared_ptr<RandomGenerator> randomGen = interpreter.getRandomGenerator();
    size_t randomIndex = randomGen->next(freeSize);
    return valueList[randomIndex];
  } else {
    // Expecting the second argument to be an integer specifying the number of
    // choices Extract n from arguments[1]
    auto nNumber = std::dynamic_pointer_cast<AutumnNumber>(arguments[1]);
    if (!nNumber) {
      throw Error("uniformChoice() argument 2 must be an integer.");
    }

    int n = nNumber->getNumber(); // Assuming AutumnNumber has getValue()

    if (n < 1) {
      throw Error("uniformChoice() argument 2 must be a positive integer.");
    }
    // Create a list to hold the selected values
    auto selectedList = std::make_shared<AutumnList>();

    std::shared_ptr<RandomGenerator> randomGen = interpreter.getRandomGenerator();
    for (int i = 0; i < n; ++i) {
      size_t randomIndex = randomGen->next(freeSize);
      selectedList->add(valueList[randomIndex]); // Assuming add method exists
    }

    return selectedList;
  }
}

int UniformChoice::arity() {
  // arity is 1 or 2, but since C++ doesn't support multiple arity directly,
  // return the maximum arity. Alternatively, adjust our arity logic as needed.
  return 2;
}

} // namespace Autumn
