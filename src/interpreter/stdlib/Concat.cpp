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
Concat::call(Interpreter &interpreter,
             const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 1) {
    throw Error("Concat() takes 1 arguments");
  }
  std::shared_ptr<AutumnList> list =
      std::dynamic_pointer_cast<AutumnList>(arguments[0]);
  if (list == nullptr) {
    throw Error("Concat() arguments must be a list of lists, instead got " +
                arguments[0]->toString());
  }

  std::shared_ptr<AutumnList> pNewList = std::make_shared<AutumnList>();
  pNewList->getValues()->reserve(list->getValues()->size());
  for (auto &elem : *(list->getValues())) {
    std::shared_ptr<AutumnList> pElemList =
        std::dynamic_pointer_cast<AutumnList>(elem);
    if (pElemList == nullptr) {
      throw Error("Concat() arguments must be a list of lists");
    }
    pNewList->getValues()->insert(pNewList->getValues()->end(),
                                  pElemList->getValues()->begin(),
                                  pElemList->getValues()->end());
  }
  return pNewList;
}

int Concat::arity() { return 2; }
} // namespace Autumn
