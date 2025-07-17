#ifndef __AUTUMN_INSTANCE_HPP__
#define __AUTUMN_INSTANCE_HPP__
#include "AutumnClass.hpp"
#include "AutumnValue.hpp"
#include <memory>

namespace Autumn {
class AutumnInstance : public AutumnValue,
                       public std::enable_shared_from_this<AutumnInstance> {
private:
  std::shared_ptr<AutumnClass> aclass;
  std::unordered_map<std::string, std::shared_ptr<AutumnValue>> fields;

public:
  AutumnInstance(int instId, std::shared_ptr<AutumnClass> aclass,
                 std::vector<std::shared_ptr<AutumnValue>> fieldvalues)
      : AutumnValue(instId, std::any(), aclass), aclass(aclass) {
    const std::vector<std::string> &fieldnames = aclass->getFieldNames();
    if (fieldnames.size() != fieldvalues.size()) {
      throw Error("Error initializing class " + aclass->name +
                  ": Field names and values do not match: " +
                  std::to_string(fieldnames.size()) + " vs " +
                  std::to_string(fieldvalues.size()));
    }
    for (size_t i = 0; i < fieldnames.size(); i++) {
      if (aclass->getFieldTypes()[i] != nullptr &&
          fieldvalues[i]->getType()->toString() !=
              aclass->getFieldTypes()[i]->toString() && fieldvalues[i]->getType()->toString() != "List<Unknown>") {
        std::cerr << "Field type mismatch: " << fieldnames[i] << " with type "
                  << aclass->getFieldTypes()[i]->toString() << " vs "
                  << fieldvalues[i]->toString() << std::endl;
        throw Error("Field type mismatch: " + fieldnames[i] + " with type " +
                    fieldvalues[i]->getType()->toString() + " vs " +
                    aclass->getFieldTypes()[i]->toString());
      }
      this->fields[fieldnames[i]] = fieldvalues[i];
    }
  }

  AutumnInstance(std::shared_ptr<AutumnClass> aclass,
                 std::vector<std::shared_ptr<AutumnValue>> fieldvalues)
      : AutumnValue(std::any(), aclass), aclass(aclass) {
    const std::vector<std::string> &fieldnames = aclass->getFieldNames();
    if (fieldnames.size() != fieldvalues.size()) {
      throw Error("Field names and values do not match in class " +
                  aclass->name + ": " + std::to_string(fieldnames.size()) +
                  " vs " + std::to_string(fieldvalues.size()));
    }

    for (size_t i = 0; i < fieldnames.size(); i++) {
      if (aclass->getFieldTypes()[i] != nullptr &&
          fieldvalues[i]->getType()->toString() !=
              aclass->getFieldTypes()[i]->toString() && fieldvalues[i]->getType()->toString() != "List<Unknown>"
              && aclass->getFieldTypes()[i]->toString() != "List<Unknown>"
              ) {
        std::cerr << "Field type mismatch: " << fieldnames[i] << " with type "
                  << aclass->getFieldTypes()[i]->toString() << " vs "
                  << fieldvalues[i]->toString() << std::endl;
        throw Error("Field type mismatch: " + fieldnames[i] + " with type " +
                    fieldvalues[i]->getType()->toString() + " vs " +
                    aclass->getFieldTypes()[i]->toString());
      }
      this->fields[fieldnames[i]] = fieldvalues[i];
    }
  }

  std::string toString() const override {
    std::string result = aclass->name + "{";
    int i = 0;
    for (const auto &key : aclass->getFieldNames()) {
      std::shared_ptr<AutumnValue> value = fields.at(key);
      if (value->getType()->toString() !=
          aclass->getFieldTypes()[i]->toString() && aclass->getFieldTypes()[i]->toString() != "List<Unknown>"
          && value->getType()->toString() != "List<Unknown>"
          ) {
        throw Error("Field type mismatch in toString: " + key + " with type " +
                    aclass->getFieldTypes()[i]->toString() + " vs " +
                    value->toString() + " in " + aclass->name);
      }
      std::string valueString = value->toString();
      // for each line, add a tab
      std::string tabbedValueString = "";
      for (const auto &line : valueString) {
        tabbedValueString += line;
        if (line == '\n') {
          tabbedValueString += "\t";
        }
      }
      result += (i == 0 ? "" : ",") + key + ": " + tabbedValueString;
      i++;
    }
    if (fields.size() == 0) {
      result += "None";
    }
    if (aclass->methods.size() != 0) {
      result += "\tMethods: ";
      for (const auto &method : aclass->methods) {
        result += "\n\t" + method.first + ": " + method.second->toString();
      }
    }
    result += "}";
    return result;
  }

  static std::string normalizeName(const std::string &name) {
    // if start with \" or ', remove them
    if (name[0] == '\"' || name[0] == '\'') {
      return name.substr(1, name.size() - 2);
    }
    return name;
  }

  std::string getClassName() { return aclass->name; }

  std::shared_ptr<AutumnValue> get(const std::string &name) {
    std::string normalizedName = normalizeName(name);
    if (fields.find(normalizedName) != fields.end()) {
      // std::cerr << "Field found: " << normalizedName << std::endl;
      return fields[normalizedName];
    }
    /// Currently Autumn does not allow method call in class
    try {
      std::shared_ptr<AutumnMethod> method = aclass->findMethod(normalizedName);
      if (method != nullptr) {
        method->setInstance(shared_from_this());
        // std::cerr << "Method found: " << std::endl;
        // std::cerr << method->toString() << std::endl;
        auto pCallableValue =
            std::dynamic_pointer_cast<AutumnCallableValue>(method);
        // std::cerr << "pCallableValue = " << pCallableValue->toString()
        //           << std::endl;
        return pCallableValue;
      } else {
        throw Error("Undefined property '" + name + "' for class " +
                    aclass->name);
      }
    } catch (Error &e) {
      throw Error("Undefined property '" + name + "' for class " +
                  aclass->name + " - " + e.what());
    }
    return nullptr;
  }

  void set(const std::string &name, std::shared_ptr<AutumnValue> value) {
    std::string normalizedName = normalizeName(name);
    if (this->fields.find(normalizedName) != fields.end()) {
      this->fields[normalizedName] = value;
      // std::cerr << "All fields: ";
      // for (const auto &[key, value] : fields) {
      //   std::cerr << key << ", ";
      //   std::cerr << value->toString() << std::endl;
      // }
      return;
    }
    throw Error("AutumnInstance::Set - Undefined property '" + normalizedName +
                "', all fields: " + aclass->toString());
  }

  bool isEqual(std::shared_ptr<AutumnValue> other) override {
    std::shared_ptr<AutumnInstance> otherInstance =
        std::dynamic_pointer_cast<AutumnInstance>(other);
    if (otherInstance == nullptr) {
      return false;
    }
    // Else, check matching in terms of fields
    if (fields.size() != otherInstance->fields.size()) {
      return false;
    }
    for (const auto &[key, value] : fields) {
      if (otherInstance->fields.find(key) == otherInstance->fields.end()) {
        return false;
      }
      if (!value->isEqual(otherInstance->fields[key])) {
        return false;
      }
    }
    return true;
  }

  std::shared_ptr<AutumnValue> clone() override {
    // TODO: Check if this breaks things.
    // std::vector<std::shared_ptr<AutumnValue>> fieldvalues;
    // fieldvalues.reserve(fields.size());
    // for (auto &key : aclass->getFieldNames()) {
    //   fieldvalues.push_back(fields[key]->clone());
    // }
    // return std::make_shared<AutumnInstance>(instId, aclass, fieldvalues);
    return shared_from_this();
  }

  std::shared_ptr<AutumnValue> copy() override {
    std::vector<std::shared_ptr<AutumnValue>> fieldvalues;
    fieldvalues.reserve(fields.size());
    for (auto &key : aclass->getFieldNames()) {
      fieldvalues.push_back(fields[key]->clone());
    }

    return std::make_shared<AutumnInstance>(aclass, fieldvalues);
  }

  std::shared_ptr<AutumnClass> getClass() { return aclass; }
};

} // namespace Autumn

#endif // !__AUTUMN_INSTANCE_HPP__
