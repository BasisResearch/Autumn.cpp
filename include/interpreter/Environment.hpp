#ifndef _AUTUMN_ENVIRONMENT_HPP
#define _AUTUMN_ENVIRONMENT_HPP

#include "AutumnType.hpp"
#include "AutumnValue.hpp"
#include "Token.hpp"
#include <any>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <regex>
enum class EnvironmentType {
  GLOBAL,
  LOCAL,
  INSTANCE,
};
namespace Autumn {
class Environment;

using EnvironmentPtr = std::shared_ptr<Environment>;
struct pair_hash {
  inline std::size_t operator()(const std::pair<int, int> &v) const {
    return v.first * 31 + v.second;
  }
};

class Environment : public std::enable_shared_from_this<Environment> {
private:
  std::unordered_map<std::string, std::shared_ptr<AutumnValue>> values;

  std::unordered_map<std::string, bool> updateStates;
  std::unordered_map<std::string, std::shared_ptr<AutumnType>> typeValues;
  std::unordered_map<std::string, std::any> assignedTypes;

  std::vector<std::string> definitionOrder;

  std::unordered_set<std::pair<int, int>, pair_hash> occupiedPositions;
  EnvironmentType environmentType;

protected:
  EnvironmentPtr enclosing;
  

public:
  // Constructors

  // Default constructor: no enclosing environment (e.g., global scope)
  Environment();

  // Constructor with an enclosing environment
  Environment(EnvironmentPtr enclosingEnv, EnvironmentType environmentType=EnvironmentType::LOCAL);

  const std::vector<std::string> &getDefinitionOrder() {
    return definitionOrder;
  }

  void clearOccupied() { occupiedPositions.clear(); }

  void occupyPosition(int x, int y) { occupiedPositions.insert({x, y}); }

  bool isFreePos(int x, int y) {
    return occupiedPositions.find({x, y}) == occupiedPositions.end();
  }

  // Define a new variable in the current environment
  void define(std::string name, std::shared_ptr<AutumnValue> value);

  bool isDefined(const std::string &name) {
    return values.find(name) != values.end();
  }

  void defineType(std::string name, std::shared_ptr<AutumnType> type);
  // Get the value of a variable at a certain distance in the environment
  // chain
  std::shared_ptr<AutumnValue> getAt(int distance, const std::string &name);

  // Assign a new value to a variable at a certain distance in the environment
  // chain
  void assignAt(int distance, const Token &name,
                const std::shared_ptr<AutumnValue> &value);

  // Retrieve the value of a variable, searching enclosing environments if
  // necessary
  std::shared_ptr<AutumnValue> get(const Token &name);
  std::shared_ptr<AutumnValue> get(const std::string &name);

  std::shared_ptr<AutumnType> getTypeValue(const Token &name);

  // Assign a new value to a variable, searching enclosing environments if
  // necessary
  void assign(const Token &name, const std::shared_ptr<AutumnValue> &value);
  void assign(const std::string &name,
              const std::shared_ptr<AutumnValue> &value);

  void assignType(std::string name, const std::shared_ptr<AutumnType> &type);

  std::shared_ptr<Environment> getEnclosing() { return enclosing; }

  std::shared_ptr<AutumnType> getAssignedType(std::string name) {
    if (assignedTypes.find(name) != assignedTypes.end()) {
      return std::any_cast<std::shared_ptr<AutumnType>>(assignedTypes[name]);
    }
    if (enclosing != nullptr) {
      return enclosing->getAssignedType(name);
    }
    return nullptr;
  };

  void resetUpdateStates();
  std::string printAllAssignedTypes() {
    std::string result = "";
    for (const auto &[key, value] : assignedTypes) {
      result += key + " : " +
                std::any_cast<std::shared_ptr<AutumnType>>(value)->toString() +
                "\n";
    }
    return result;
  }

  bool isUpdated(const std::string &name) {
    if (updateStates.find(name) != updateStates.end()) {
      return updateStates[name];
    }
    if (enclosing != nullptr) {
      return enclosing->isUpdated(name);
    }
    return false;
  }

  std::string printAllDefinedVariables() {
    std::string result = "";
    for (const auto &[key, value] : values) {
      result += key + " : " + value->toString() + "\n";
    }
    return result;
  }

  std::string printAllDefinedVariablesCrossStack() {
    std::string result = "";
    for (const auto &[key, value] : values) {
      result += key + " : " + value->toString() + "\n";
    }
    if (enclosing != nullptr) {
      result += enclosing->printAllDefinedVariablesCrossStack();
    }
    return result;
  }

  const std::unordered_map<std::string, std::shared_ptr<AutumnValue>> &
  getDefinedVariables() {
    return values;
  }

  std::string printAllTypeValues() {
    std::string result = "";
    for (const auto &[key, value] : typeValues) {
      result += key + " : " +
                std::any_cast<std::shared_ptr<AutumnType>>(value)->toString() +
                "\n";
    }
    return result;
  }

  EnvironmentPtr copy(EnvironmentPtr copiedEnclosing) {
    EnvironmentPtr newEnv = std::make_shared<Environment>(copiedEnclosing, environmentType);
    // Deep copy value
    std::unordered_map<std::string, std::shared_ptr<AutumnValue>> values;
    for (const auto &[key, value] : this->values) {
      values[key] = value->clone();
    }
    newEnv->values = values;
    // Deep copy update states
    std::unordered_map<std::string, bool> updateStates;
    for (const auto &[key, value] : this->updateStates) {
      updateStates[key] = value;
    }
    // Deep copy type values
    std::unordered_map<std::string, std::shared_ptr<AutumnType>> typeValues;
    for (const auto &[key, value] : this->typeValues) {
      typeValues[key] = value;
    }
    newEnv->updateStates = updateStates;
    newEnv->assignedTypes = assignedTypes;
    newEnv->typeValues = typeValues;
    return newEnv;
  }

  void copyAll(EnvironmentPtr from) {
    for (const auto &[key, value] : from->values) {
      values[key] = value->clone();
    }
    for (const auto &[key, value] : from->typeValues) {
      typeValues[key] = value;
    }
    for (const auto &[key, value] : from->updateStates) {
      updateStates[key] = value;
    }
    for (const auto &[key, value] : from->assignedTypes) {
      assignedTypes[key] = value;
    }
    for (auto key : from->definitionOrder) {
      definitionOrder.push_back(key);
    }
  }

  void selectiveCopy(EnvironmentPtr from, std::vector<std::string> keys) {
    for (const auto &[key, value] : from->values) {
      if (std::find(keys.begin(), keys.end(), key) != keys.end()) {
        values[key] = value->clone();
      }
    }
    for (const auto &[key, value] : from->typeValues) {
      typeValues[key] = value;
    }
    for (const auto &[key, value] : from->updateStates) {
      if (std::find(keys.begin(), keys.end(), key) != keys.end()) {
        updateStates[key] = value;
      }
    }
    for (const auto &[key, value] : from->assignedTypes) {
      if (std::find(keys.begin(), keys.end(), key) != keys.end()) {
        assignedTypes[key] = value;
      }
    }
    for (auto key : from->definitionOrder) {
      if (std::find(keys.begin(), keys.end(), key) != keys.end()) {
        definitionOrder.push_back(key);
      }
    }
  }


  void setEnclosing(EnvironmentPtr enclosing) { this->enclosing = enclosing; }

  std::shared_ptr<AutumnValue> findId(int instId) {
    for (const auto &[key, value] : values) {
      if (value->getInstId() == instId) {
        return value;
      }
    }
    if (enclosing != nullptr) {
      return enclosing->findId(instId);
    }
    return nullptr;
  }

  bool removeIdIfExist(int instId) {
    for (const auto &[key, value] : values) {
      if (value->getInstId() == instId) {
        values.erase(key);
        return true;
      }
    }
    if (enclosing != nullptr) {
      return enclosing->removeIdIfExist(instId);
    }
    return false;
  }

  EnvironmentType getEnvironmentType() { return environmentType; }


  std::string toJson(std::string childScope = ""){
    // Need to encode: values, updateStates, typeValues, assignedTypes
    // std::unordered_map<std::string, std::shared_ptr<AutumnValue>> values;

    // EnvironmentPtr enclosing;
    // std::unordered_map<std::string, bool> updateStates;
    // std::unordered_map<std::string, std::shared_ptr<AutumnType>> typeValues;
    // std::unordered_map<std::string, std::any> assignedTypes;
    std::string result = "";
    std::unordered_map<std::string, std::string> varScope;
    for (const auto &[key, value] : values) {
        varScope[key] = value->toString();
    }
    std::unordered_map<std::string, std::string> typeScope;
    for (const auto &[key, value] : typeValues) {
        typeScope[key] = std::any_cast<std::shared_ptr<AutumnType>>(value)->toString();
    }
    // Indent child scope
    std::string indent = "   ";
    std::vector<std::string> childScopeLines;
    std::stringstream ss(childScope);
    std::string line;
    while (std::getline(ss, line)) {
      childScopeLines.push_back(line);
    }
    for (int i = 0; i < childScopeLines.size(); i++) {
      childScopeLines[i] = indent + childScopeLines[i];
    }
    result += "{\n";
    result += indent + "\"varValues\": {\n";
    for (const auto &[key, value] : varScope) {
      std::string valueStr = value;
      // Sanitize the value string to be a valid JSON string
      valueStr = std::regex_replace(valueStr, std::regex("\\n"), "\\n");
      valueStr = std::regex_replace(valueStr, std::regex("\\r"), "\\r");
      valueStr = std::regex_replace(valueStr, std::regex("\\t"), "\\t");
      valueStr = std::regex_replace(valueStr, std::regex("\""), "\\\"");
      result += indent + "    \"" + key + "\": \"" + valueStr + "\",\n";
    }
    result += indent + "  },\n";
    result += indent + "\"typeValues\": {\n";
    for (const auto &[key, value] : typeScope) {
      result += indent + "    \"" + key + "\": \"" + value + "\",\n";
    }
    result += indent + "  }" + (childScope != "" ? ",\n" : "\n");
    if (childScope != "") { // Only add child scope if it is not empty
      result += indent + "\"childScope\": \n";
      for (const auto &line : childScopeLines) {
        result += line + "\n";
      }
    }
    result += "}\n";
    return result;
  }

  void reset() {
    values.clear();
    updateStates.clear();
    typeValues.clear();
    assignedTypes.clear();
    definitionOrder.clear();
  }

private:
  // Helper function to traverse to the ancestor environment at a given distance
  EnvironmentPtr ancestor(int distance);
};
} // namespace Autumn

#endif
