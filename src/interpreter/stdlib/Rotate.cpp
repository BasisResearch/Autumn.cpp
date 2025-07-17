#include "AutumnCallableValue.hpp"
#include "AutumnClass.hpp"
#include "AutumnExprValue.hpp"
#include "AutumnInstance.hpp"
#include "AutumnLambda.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

namespace Autumn {
std::shared_ptr<AutumnValue>
Rotate::call(Interpreter &interpreter,
             const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 1) {
    throw Error("Rotate() takes 1 argument");
  }
  auto instance =
      std::dynamic_pointer_cast<AutumnInstance>(arguments[0]->copy());
  if (instance == nullptr) {
    throw Error("Rotate() argument must be an Object instance");
  }
  auto elems = instance->get("elems");
  if (elems == nullptr) {
    throw Error("Rotate() argument must have an 'elems' attribute");
  }
  auto list = std::dynamic_pointer_cast<AutumnList>(elems);
  if (list == nullptr) {
    throw Error("Rotate() argument 'elems' attribute must be a list");
  }
  auto subenv = std::make_shared<Environment>(interpreter.getEnvironment());

  interpreter.setEnvironment(subenv);
  auto fieldnames = instance->getClass()->getFieldNames();
  for (auto &fieldname : fieldnames) {
    if (fieldname == "elems")
      continue;
    subenv->define(fieldname, instance->get(fieldname));
  }
  SExpParser tmpParser = SExpParser("");
  for (auto &value : *list->getValues()) {
    auto subinstance = std::dynamic_pointer_cast<AutumnInstance>(value);
    if (subinstance == nullptr) {
      throw Error("Rotate() argument 'elems' attribute must contain only "
                  "Object instances");
    }
    auto xExpr =
        std::dynamic_pointer_cast<AutumnExprValue>(subinstance->get("x"));
    if (xExpr == nullptr) {
      throw Error("Rotate() argument 'elems' attribute Object instances must "
                  "have a 'x' attribute");
    }
    auto yExpr =
        std::dynamic_pointer_cast<AutumnExprValue>(subinstance->get("y"));
    if (yExpr == nullptr) {
      throw Error("Rotate() argument 'elems' attribute Object instances must "
                  "have a 'y' attribute");
    }

    auto new_x = std::make_shared<AutumnExprValue>(std::make_shared<Unary>(
        Token(TokenType::MINUS, "-", nullptr, 0), yExpr->getExpr()), xExpr->getCenv());
    subinstance->set("x", new_x);
    subinstance->set("y", xExpr);
  }
  interpreter.setEnvironment(subenv->getEnclosing());
  return instance;
}

} // namespace Autumn
