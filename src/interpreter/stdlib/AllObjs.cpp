#include "AutumnCallableValue.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

namespace Autumn {

static std::shared_ptr<AutumnList>
getAllObjs(Interpreter &interpreter, std::shared_ptr<AutumnValue> value) {
  auto pList = std::dynamic_pointer_cast<AutumnList>(value);
  auto pInstance = std::dynamic_pointer_cast<AutumnInstance>(value);
  if (pList != nullptr) {
    std::vector<std::shared_ptr<AutumnValue>> values;
    values.reserve(pList->getValues()->size() * 3);
    for (auto &elem : *(pList->getValues())) {
      auto pObjs = getAllObjs(interpreter, elem);
      for (auto &obj : *(pObjs->getValues())) {
        auto pInstance = std::dynamic_pointer_cast<AutumnInstance>(obj);
        if (pInstance->getClass()->findMethod("render") != nullptr) {
          values.push_back(obj);
        }
      }
    }
    return std::make_shared<AutumnList>(
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(values));
  } else if (pInstance != nullptr) {
    auto retVal = std::make_shared<AutumnList>();
    if (pInstance->getClass()->findMethod("render") != nullptr) {
      retVal->add(pInstance);
    }
    return retVal;
  } else {
    return std::make_shared<AutumnList>(
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(
            std::vector<std::shared_ptr<AutumnValue>>({})));
  }
}

std::shared_ptr<AutumnValue>
AllObjs::call(Interpreter &interpreter,
              const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  auto pList = std::make_shared<AutumnList>();
  pList->getValues()->reserve(interpreter.getGlobals()->getDefinedVariables().size() * 3);
  for (auto &kv : interpreter.getGlobals()->getDefinedVariables()) {
    auto allObjs = getAllObjs(interpreter, kv.second);
    pList->getValues()->insert(pList->getValues()->end(),
                               allObjs->getValues()->begin(),
                               allObjs->getValues()->end());
  }
  return pList;
}
} // namespace Autumn
