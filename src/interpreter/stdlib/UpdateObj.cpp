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
std::shared_ptr<AutumnValue>
UpdateObj::call(Interpreter &interpreter,
                const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() > 3 || arguments.size() < 2) {
    throw Error("UpdateObj() takes at least 2 and at most 3 arguments");
  }
  std::shared_ptr<AutumnInstance> obj =
      std::dynamic_pointer_cast<AutumnInstance>(arguments[0]);
  std::shared_ptr<AutumnList> list =
      std::dynamic_pointer_cast<AutumnList>(arguments[0]);
  if (obj == nullptr && list == nullptr) {
    throw Error("UpdateObj() first argument must be an instance or a list");
  }
  if (list != nullptr) {
    if (arguments.size() == 2) {
      Map map = Map();
      std::vector<std::shared_ptr<AutumnValue>> args = {arguments[1], list};
      try {
        std::shared_ptr<AutumnValue> retVal = map.call(interpreter, args);
        return retVal;
      } catch (const Error &e) {
        throw Error(std::string("Error interpreting UpdateObj - Got\n") +
                    e.what());
      }
    } else if (arguments.size() == 3) {
      auto apply_func =
          std::dynamic_pointer_cast<AutumnCallableValue>(arguments[1]);
      auto filter_func =
          std::dynamic_pointer_cast<AutumnCallableValue>(arguments[2]);
      if (apply_func != nullptr && filter_func != nullptr) {
        std::shared_ptr<AutumnList> retVal = std::make_shared<AutumnList>();
        for (auto &val : *list->getValues()) {
          std::vector<std::shared_ptr<AutumnValue>> args = {val};
          std::vector<std::shared_ptr<AutumnValue>> filter_args = args;
          std::shared_ptr<AutumnValue> filter_ret =
              filter_func->call(interpreter, filter_args);
          if (filter_ret->isTruthy()) {
            std::shared_ptr<AutumnValue> apply_ret =
                apply_func->call(interpreter, args);
            retVal->add(apply_ret);
          } else {
            retVal->add(val);
          }
        }
        return retVal;
      } else {
        throw Error("UpdateObj() second and third arguments must be "
                    "callables,instead got " +
                    arguments[1]->toString() + " and " +
                    arguments[2]->toString());
      }
    }
  } else {
    if (arguments.size() == 3) {
      std::shared_ptr<AutumnValue> value = arguments[2];
      std::shared_ptr<AutumnValue> retVal = obj->copy();
      std::shared_ptr<AutumnString> sval =
          std::dynamic_pointer_cast<AutumnString>(arguments[1]);
      if (sval == nullptr) {
        throw Error("UpdateObj() second argument must be a string");
      }
      std::shared_ptr<AutumnInstance> retObj =
          std::dynamic_pointer_cast<AutumnInstance>(retVal);
      if (retObj == nullptr) {
        throw Error("UpdateObj() first argument must be an instance");
      }
      retObj->set(sval->getString(), value);
      return retObj;
    } else {
      std::shared_ptr<AutumnCallableValue> callable =
          std::dynamic_pointer_cast<AutumnCallableValue>(arguments[1]);
      if (callable == nullptr) {
        throw Error("UpdateObj() second argument must be a callable");
      }
      std::shared_ptr<AutumnValue> retVal = callable->call(interpreter, {obj});
      return retVal;
    }
  }
  throw Error("UpdateObj() first argument must be an instance or a list, "
              "instead got " +
              arguments[0]->toString());
}

int UpdateObj::arity() { return 2; }
} // namespace Autumn
