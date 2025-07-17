#ifndef __AUTUMN_CALLABLE_VALUE_HPP__
#define __AUTUMN_CALLABLE_VALUE_HPP__

#include "AstPrinter.hpp"
#include "AutumnCallable.hpp"
#include "AutumnValue.hpp"
#include "Interpreter.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

namespace Autumn {
class AutumnFuncType : public AutumnType {

public:
  static std::shared_ptr<AutumnFuncType> instance;
  std::string toString() const { return std::string("AutumnFunc"); }
  static std::shared_ptr<AutumnFuncType> getInstance() {
    if (instance == nullptr) {
      instance = std::make_shared<AutumnFuncType>();
    }
    return instance;
  }
};

class AutumnCallableValue : public AutumnValue,
                            public std::enable_shared_from_this<AutumnCallableValue> {

  std::string callableString;

public:
  std::shared_ptr<AutumnCallable> callable;
  AutumnCallableValue(std::shared_ptr<AutumnCallable> callable)
      : AutumnValue(callable->clone(), AutumnFuncType::getInstance()),
        callable(callable->clone()) {
    if (callable == nullptr) {
      throw Error("Error Initializing AutumnCallableValue: callable is null");
    }
    AstPrinter printer;
    callableString = std::any_cast<std::string>(callable->toString());
  }

  AutumnCallableValue(int instId, std::shared_ptr<AutumnCallable> callable)
      : AutumnValue(instId, callable->clone(), AutumnFuncType::getInstance()),
        callable(callable->clone()) {
    if (callable == nullptr) {
      throw Error(
          "Error Initializing Cloned AutumnCallableValue: callable is null");
    }
    AstPrinter printer;
    callableString = std::any_cast<std::string>(callable->toString());
  }

  bool isEqual(std::shared_ptr<AutumnValue> other) override {
    std::shared_ptr<AutumnCallableValue> otherLambda =
        std::dynamic_pointer_cast<AutumnCallableValue>(other);
    if (otherLambda == nullptr) {
      return false;
    }
    return callable == otherLambda->callable;
  }

  virtual std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
    return callable->call(interpreter, arguments);
  }
  std::string toString() const override {
    if (callable == nullptr) {
      throw Error("Error in CallableValue::toString - callable is null");
    }
    return std::string("<native fn: ") + callable->toString() + ">";
  }

  std::shared_ptr<AutumnValue> clone() override {
    if (callable == nullptr) {
      throw Error("Error in CallableValue::clone - callable is null");
    }
    // Return self instead to save memory and avoid extra clone
    return shared_from_this();
  }

  std::shared_ptr<AutumnValue> copy() override {
    if (callable == nullptr) {
      throw Error("Error in CallableValue::copy - callable is null");
    }
    return shared_from_this();
  }
};
} // namespace Autumn

#endif // __AUTUMN_CALLABLE_VALUE_HPP__
