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
AddObj::call(Interpreter &interpreter,
             const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 2) {
    throw Error("AddObj() takes 2 arguments");
  }
  std::shared_ptr<AutumnList> list =
      std::dynamic_pointer_cast<AutumnList>(arguments[0]);
  if (list == nullptr) {
    throw Error("AddObj() first argument must be a list");
  }
  std::shared_ptr<AutumnList> newList =
      std::dynamic_pointer_cast<AutumnList>(list->copy());
  std::shared_ptr<AutumnInstance> obj =
      std::dynamic_pointer_cast<AutumnInstance>(arguments[1]);
  std::shared_ptr<AutumnList> newListObj =
      std::dynamic_pointer_cast<AutumnList>(arguments[1]);
  if (obj == nullptr && newListObj == nullptr) {
    throw Error("AddObj() second argument must be an instance, instead got " +
                arguments[1]->toString());
  }
  if (obj != nullptr) {
    newList->add(obj);
    return newList;
  } else {
    newList->getValues()->reserve(newListObj->getValues()->size());
    // FIXME: check the type of two lists first.
    // Trust that all elements in newListObj are instances due to add checking in the former list.
    newList->getValues()->insert(newList->getValues()->end(),
                                 newListObj->getValues()->begin(),
                                 newListObj->getValues()->end());
    return newList;
  }
}

int AddObj::arity() { return 2; }
} // namespace Autumn
