#ifndef __AUTUMN_CLASS_HPP__
#define __AUTUMN_CLASS_HPP__
#include "AutumnCallable.hpp"
#include "AutumnCallableValue.hpp"
#include "AutumnType.hpp"
namespace Autumn {

class AutumnInstance;

class AutumnMethod : public AutumnCallableValue {
  std::shared_ptr<AutumnInstance> instance;

  static std::shared_ptr<AutumnCallable>
  checkNull(std::shared_ptr<AutumnCallable> method) {
    if (method == nullptr) {
      throw Error("Error Initializing AutumnMethod: method is null");
    }
    return method;
  }

public:
  AutumnMethod(int instId, std::shared_ptr<AutumnCallable> method)
      : AutumnCallableValue(instId, checkNull(method)), instance(nullptr) {
    AutumnCallableValue(instId, checkNull(method));
  }

  AutumnMethod(std::shared_ptr<AutumnCallable> method)
      : AutumnCallableValue(checkNull(method)), instance(nullptr) {}

  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override;

  std::string toString() const override;

  bool isEqual(std::shared_ptr<AutumnValue> other) override {
    std::shared_ptr<AutumnMethod> otherMethod =
        std::dynamic_pointer_cast<AutumnMethod>(other);
    if (otherMethod == nullptr) {
      return false;
    }
    return callable == otherMethod->callable;
  }

  void setInstance(std::shared_ptr<AutumnInstance> instance) {
    if (instance == nullptr) {
      std::cerr << "Error Binding Method Instance: instance is null"
                << std::endl;
      throw Error("Error Binding Method Instance: instance is null");
    }
    this->instance = instance;
  }

  std::shared_ptr<AutumnValue> clone() override {
    return shared_from_this();
  }

  std::shared_ptr<AutumnValue> copy() override {
    return shared_from_this();
  }
};

class AutumnClass : public AutumnType {
protected:
  std::vector<std::string> fieldnames;
  std::vector<std::shared_ptr<AutumnType>> fieldtypes;
  std::shared_ptr<AutumnCallable> initializer;

public:
  std::string name;
  std::unordered_map<std::string, std::shared_ptr<AutumnMethod>> methods;

  AutumnClass(
      std::string name, std::vector<std::string> fieldnames,
      std::vector<std::shared_ptr<AutumnType>> fieldtypes,
      std::shared_ptr<AutumnCallable> initializer,
      std::unordered_map<std::string, std::shared_ptr<AutumnMethod>> methods)
      : name(name), methods(methods), initializer(initializer),
        fieldnames(fieldnames), fieldtypes(fieldtypes) {}

  std::shared_ptr<AutumnMethod> findMethod(const std::string &name) {
    if (methods.find(name) != methods.end()) {
      return methods[name];
    }
    return nullptr;
  }

  const std::vector<std::string> &getFieldNames() { return fieldnames; }
  std::shared_ptr<AutumnCallable> getInitializer() { return initializer; }
  std::string toString() const override {
    std::string results = "class " + name + " {";
    for (size_t i = 0; i < fieldnames.size(); i++) {
      results += fieldnames[i] + ": " + fieldtypes[i]->toString() + ", ";
    }
    results += "}";
    // results += " having initializer: " +
    //            (initializer == nullptr ? "None" : initializer->toString());
    return results;
  }

  const std::vector<std::shared_ptr<AutumnType>> &getFieldTypes() {
    return fieldtypes;
  }
};

} // namespace Autumn

#endif // !__AUTUMN_CLASS_HPP__
