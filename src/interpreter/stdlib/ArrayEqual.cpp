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
ArrayEqual::call(Interpreter &interpreter,
                 const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 2) {
    throw Error("ArrayEqual() takes 2 arguments");
  }

  std::shared_ptr<AutumnList> list1 =
      std::dynamic_pointer_cast<AutumnList>(arguments[0]);
  std::shared_ptr<AutumnList> list2 =
      std::dynamic_pointer_cast<AutumnList>(arguments[1]);

  if (list1 == nullptr || list2 == nullptr) {
    throw Error("ArrayEqual() arguments must be lists");
  }

  auto values1 = list1->getValues();
  auto values2 = list2->getValues();

  if (values1->size() != values2->size()) {
    throw Error("ArrayEqual() lists must have the same length");
  }

  std::shared_ptr<AutumnList> result = std::make_shared<AutumnList>();
  
  for (size_t i = 0; i < values1->size(); i++) {
    bool isEqual = (*values1)[i]->isEqual((*values2)[i]);
    result->add(std::make_shared<AutumnBool>(isEqual));
  }

  return result;
}

} // namespace Autumn 