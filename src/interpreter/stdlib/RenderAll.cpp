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

namespace Autumn {

static std::shared_ptr<AutumnList>
renderValue(Interpreter &interpreter, std::shared_ptr<AutumnValue> value) {
  auto pList = std::dynamic_pointer_cast<AutumnList>(value);
  auto pInstance = std::dynamic_pointer_cast<AutumnInstance>(value);
  if (pList != nullptr) {
    std::vector<std::shared_ptr<AutumnValue>> values;
    values.reserve(pList->getValues()->size() * 3);
    for (auto &elem : *(pList->getValues())) {
      auto pRendered = renderValue(interpreter, elem);
      values.insert(values.end(), pRendered->getValues()->begin(),
                    pRendered->getValues()->end());
    }
    return std::make_shared<AutumnList>(
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(values));
  } else if (pInstance != nullptr) {
    if (pInstance->getClass()->findMethod("render") == nullptr) {
      // std::cerr << "Warning: Instance does not have render method" <<
      // std::endl;
      return std::make_shared<AutumnList>(
          std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(
              std::vector<std::shared_ptr<AutumnValue>>()));
    }
    std::shared_ptr<AutumnCallableValue> render =
        std::dynamic_pointer_cast<AutumnCallableValue>(
            pInstance->get("render"));
    std::shared_ptr<AutumnValue> result = render->call(interpreter, {});
    return std::dynamic_pointer_cast<AutumnList>(result);
  } else {
    return std::make_shared<AutumnList>(
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(
            std::vector<std::shared_ptr<AutumnValue>>({})));
  }
}

std::shared_ptr<AutumnValue>
RenderAll::call(Interpreter &interpreter,
                const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  auto pList = std::make_shared<AutumnList>();
  auto mapVal = interpreter.getGlobals()->getDefinedVariables();
  auto keys = interpreter.getGlobals()->getDefinitionOrder();
  pList->getValues()->reserve(keys.size() * 3);
  for (auto &key : keys) {
    auto renderedElems = renderValue(interpreter, mapVal[key]);
    pList->getValues()->insert(pList->getValues()->end(),
                               renderedElems->getValues()->begin(),
                               renderedElems->getValues()->end());
  }
  interpreter.getGlobals()->assign("cacheRendered", pList);
  return pList;
}
} // namespace Autumn
