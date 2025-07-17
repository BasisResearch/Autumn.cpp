
#include "AutumnCallableValue.hpp"
#include "AutumnInstance.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>
#include <unordered_set>

namespace Autumn {
std::shared_ptr<AutumnValue>
RemoveObj::call(Interpreter &interpreter,
                const std::vector<std::shared_ptr<AutumnValue>>
                    &arguments) { /// NOTE: This is inplace
  if (arguments.size() < 2) {
    std::shared_ptr<AutumnValue> obj = arguments[0];
    std::shared_ptr<Environment> env = interpreter.getGlobals();
    env->removeIdIfExist(obj->getInstId());
  } else if (arguments.size() == 2) {
    std::shared_ptr<AutumnValue> obj = arguments[1];
    std::shared_ptr<AutumnCallableValue> callable =
        std::dynamic_pointer_cast<AutumnCallableValue>(arguments[1]);

    std::shared_ptr<AutumnInstance> instance =
        std::dynamic_pointer_cast<AutumnInstance>(arguments[1]);
    std::shared_ptr<AutumnList> rmList =
        std::dynamic_pointer_cast<AutumnList>(arguments[1]);
    if (callable == nullptr && instance == nullptr && rmList == nullptr) {
      throw Error(
          "RemoveObj() second argument must be a callable, instance, or "
          "list");
    }

    std::shared_ptr<AutumnList> list =
        std::dynamic_pointer_cast<AutumnList>(arguments[0]);
    if (list == nullptr) {
      throw Error("RemoveObj() first argument must be a list");
    }
    std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>> pList =
        list->getValues();

    std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>> valueList =
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>();
    valueList->reserve(pList->size());
    if (callable != nullptr) {
      for (const auto &value : *pList) {
        if (callable->call(interpreter, {value})->isTruthy()) {
          continue;
        }
        valueList->push_back(value);
      }
      std::shared_ptr<AutumnList> newList =
          std::make_shared<AutumnList>(valueList);
      return newList;
    } else if (instance != nullptr) {
      for (const auto &value : *pList) {
        if (value->getInstId() == instance->getInstId()) {
          continue;
        }
        valueList->push_back(value);
      }
      std::shared_ptr<AutumnList> newList =
          std::make_shared<AutumnList>(valueList);
      return newList;
    } else if (rmList != nullptr) {
      auto rmLists = std::unordered_set<int>();
      for (const auto &value : *rmList->getValues()) {
        rmLists.insert(value->getInstId());
      }
      for (const auto &value : *pList) {
        if (rmLists.find(value->getInstId()) != rmLists.end()) {
          continue;
        }
        valueList->push_back(value);
      }
      std::shared_ptr<AutumnList> newList =
          std::make_shared<AutumnList>(valueList);
      return newList;
    } else {
      throw Error("RemoveObj() second argument must be a callable, instance, "
                  "or list");
    }
  } else {
    throw Error("RemoveObj() takes 1 or 2 arguments");
  }
  return arguments[0];
}

int RemoveObj::arity() { return 2; }
} // namespace Autumn
