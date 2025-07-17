#include "Environment.hpp"
#include "Error.hpp"
#include <memory>

Autumn::Environment::Environment() : enclosing(nullptr), environmentType(EnvironmentType::GLOBAL) {}

Autumn::Environment::Environment(EnvironmentPtr enclosingEnv, EnvironmentType environmentType)
    : enclosing(enclosingEnv), environmentType(environmentType) {}

Autumn::EnvironmentPtr Autumn::Environment::ancestor(int distance) {
  EnvironmentPtr environment = shared_from_this();
  for (int i = 0; i < distance; i++) {
    environment = environment->enclosing;
  }
  return environment;
}

void Autumn::Environment::define(std::string name,
                                 std::shared_ptr<AutumnValue> value) {
  values[name] = value;
  definitionOrder.push_back(name);
  updateStates[name] = false;
}

void Autumn::Environment::defineType(std::string name,
                                     std::shared_ptr<AutumnType> value) {
  typeValues[name] = value;
}

std::shared_ptr<Autumn::AutumnValue>
Autumn::Environment::getAt(int distance, const std::string &name) {
  return ancestor(distance)->values[name];
}

void Autumn::Environment::assignAt(
    int distance, const Token &name,
    const std::shared_ptr<Autumn::AutumnValue> &value) {
  ancestor(distance)->values[name.lexeme] = value;
  updateStates[name.lexeme] = true;
}

std::shared_ptr<Autumn::AutumnValue>
Autumn::Environment::get(const Token &name) {
  if (values.find(name.lexeme) != values.end()) {
    return values[name.lexeme];
  }

  if (enclosing != nullptr) {
    return enclosing->get(name);
  }

  throw Error(std::string("Undefined variable '" + name.lexeme + "'. Defined arguments are: ") + this->printAllDefinedVariablesCrossStack());
}

std::shared_ptr<Autumn::AutumnValue>
Autumn::Environment::get(const std::string &name) {
  if (values.find(name) != values.end()) {
    return values[name];
  }

  if (enclosing != nullptr) {
    return enclosing->get(name);
  }

  throw Error(std::string("Undefined variable '" + name + "'. Defined arguments are: ") + this->printAllDefinedVariablesCrossStack());
}

std::shared_ptr<Autumn::AutumnType>
Autumn::Environment::getTypeValue(const Token &name) {
  if (typeValues.find(name.lexeme) != typeValues.end()) {
    return typeValues[name.lexeme];
  }

  if (enclosing != nullptr) {
    return enclosing->getTypeValue(name);
  }

  return nullptr;
}

void Autumn::Environment::assign(const Token &name,
                                 const std::shared_ptr<AutumnValue> &value) {
  return assign(name.lexeme, value);
}

void Autumn::Environment::assign(const std::string &name,
                                 const std::shared_ptr<AutumnValue> &value) {
  if (values.find(name) != values.end()) {
    auto oldType = values[name]->getType();
    auto newType = value->getType();
    if (oldType->toString() != newType->toString() &&
        std::dynamic_pointer_cast<AutumnList>(value) == nullptr) {
      throw Error(std::string("Cannot assign value of type '") +
                  newType->toString() + "' to variable of type '" +
                  oldType->toString() + "' for variable '" + name + "'.");
    }
    int oldInstId = values[name]->getInstId();
    values[name] = value;
    values[name]->setInstId(oldInstId);
    updateStates[name] = true;
    return;
  } else {
    auto ans = enclosing;
    while (ans != nullptr && ans->values.find(name) == ans->values.end()) {
      ans = ans->enclosing;
    }
    if (ans != nullptr) {
      ans->assign(name, value);
    } else {
      define(name, value);
    }
  }
}

void Autumn::Environment::assignType(std::string name,
                                     const std::shared_ptr<AutumnType> &type) {
  assignedTypes[name] = type;
}

void Autumn::Environment::resetUpdateStates() {
  for (auto &entry : updateStates) {
    entry.second = false;
  }
}
