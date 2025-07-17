#ifndef __AUTUMN_VALUE_HPP__
#define __AUTUMN_VALUE_HPP__

#include "AutumnType.hpp"
#include "Error.hpp"
#include <any>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace Autumn {
  /* // TODO: THIS IS A MUCH BIGGER OPTIMIZATION, beware of going down this path
template<typename... Types>
class ValueVariant {
    std::variant<Types...> data;
public:
    template<typename T>
    T& get() { return std::get<T>(data); }
};

using AutumnValueVariant = ValueVariant<
    int,                  // Number
    std::string,         // String
    bool,                // Bool
    std::vector<std::shared_ptr<AutumnValue>>, // List
    std::nullptr_t       // Null
>;

// TODO: Implement this with Autumn Values
*/
class AutumnValue {
  static int instCount;

protected:
  int instId;
  AutumnValue(int instId, std::any value, std::shared_ptr<AutumnType> type)
      : instId(instId), value(value), type(type) {}

  std::any value;
  std::shared_ptr<AutumnType> type;

public:
  AutumnValue(std::any value, std::shared_ptr<AutumnType> type)
      : value(value), type(type) {
    instId = instCount++;
  } // This is for inner class definition
  virtual std::string toString() const = 0;
  virtual bool isEqual(std::shared_ptr<AutumnValue> other) {
    return false;
  } // By default, return false
  virtual ~AutumnValue() = default;
  virtual bool isTruthy() { return true; }
  virtual std::shared_ptr<AutumnValue> clone() = 0;
  int getInstId() { return instId; }
  void setInstId(int instId) { this->instId = instId; }
  std::any getValue() { return value; }
  void setValue(std::any value) { this->value = value; }
  // copy
  virtual std::shared_ptr<AutumnValue> copy() = 0;
  std::shared_ptr<AutumnType> getType() { return type; }
};

class AutumnNumber final: public AutumnValue,
                     public std::enable_shared_from_this<AutumnNumber> {

public:
  AutumnNumber(int instId, int value)
      : AutumnValue(instId, value, AutumnNumberType::getInstance()) {}
  AutumnNumber(int value)
      : AutumnValue(value, AutumnNumberType::getInstance()) {}
  std::string toString() const override {
    return "(" + std::to_string(std::any_cast<int>(value)) + ": N)";
  }
  bool isEqual(std::shared_ptr<AutumnValue> other) override;
  std::shared_ptr<AutumnValue> clone() override {
    return shared_from_this();
  }

  int getNumber() const { return std::any_cast<int>(value); }
  AutumnNumber operator+(const AutumnNumber &other) const {
    return AutumnNumber(getNumber() + other.getNumber());
  }

  AutumnNumber operator-(const AutumnNumber &other) const {
    return AutumnNumber(getNumber() - other.getNumber());
  }

  AutumnNumber operator*(const AutumnNumber &other) const {
    return AutumnNumber(getNumber() * other.getNumber());
  }

  AutumnNumber operator/(const AutumnNumber &other) const {
    return AutumnNumber(getNumber() / other.getNumber());
  }

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnNumber>(std::any_cast<int>(value));
  }
};
;

class AutumnString final: public AutumnValue,
                     public std::enable_shared_from_this<AutumnString> {

public:
  AutumnString(int instId, std::string value)
      : AutumnValue(instId, value, AutumnStringType::getInstance()) {}
  AutumnString(std::string value)
      : AutumnValue(value, AutumnStringType::getInstance()) {}
  std::string toString() const override {
    return "(" + std::any_cast<std::string>(value) + ": S)";
  }

  bool isEqual(std::shared_ptr<AutumnValue> other) override;
  std::shared_ptr<AutumnValue> clone() override {
    return shared_from_this();
  }

  std::string getString() const { return std::any_cast<std::string>(value); }
  AutumnString operator+(const AutumnString &other) const {
    return AutumnString(getString() + other.getString());
  }

  AutumnString(AutumnString &&other)
      : AutumnValue(std::string(other.getString()),
                    AutumnStringType::getInstance()) {}

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnString>(
        std::string(std::any_cast<std::string>(value)));
  }
};

class AutumnBool final: public AutumnValue,
                   public std::enable_shared_from_this<AutumnBool> {
public:
  AutumnBool(int instId, std::any value)
      : AutumnValue(instId, value, AutumnBoolType::getInstance()) {}
  AutumnBool(std::any value)
      : AutumnValue(value, AutumnBoolType::getInstance()) {}
  std::string toString() const override {
    return std::string("(") + (std::any_cast<bool>(value) ? "true" : "false") +
           ": " + type->toString() + ")";
  }
  bool isEqual(std::shared_ptr<AutumnValue> other) override;
  bool isTruthy() override { return std::any_cast<bool>(value); }

  std::shared_ptr<AutumnValue> clone() override {
    return shared_from_this();
  }

  bool getBool() const { return std::any_cast<bool>(value); }
  AutumnBool(AutumnBool &other) : AutumnValue(other) {}

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnBool>(value);
  }
};

class AutumnList final: public AutumnValue,
                   public std::enable_shared_from_this<AutumnList> {
public:
  static std::shared_ptr<AutumnType>
  inferType(const std::vector<std::shared_ptr<AutumnValue>> &values) {
    std::shared_ptr<AutumnType> type = AutumnUnknownType::getInstance();
    for (size_t i = 0; i < values.size(); i++) {
      if (values[i]->getType() == AutumnUnknownType::getInstance()) {
        continue;
      }
      if (type == AutumnUnknownType::getInstance()) {
        type = values[i]->getType();
      } else if (type != values[i]->getType()) {
        return AutumnUnknownType::getInstance();
      }
    }
    return AutumnListType::getInstance(type);
  }

  AutumnList(int instId,
             std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>> values)
      : AutumnValue(instId, values, inferType(*values)) {}

  AutumnList(std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>> values)
      : AutumnValue(values, inferType(*values)) {}

  AutumnList()
      : AutumnValue(std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(
                        std::vector<std::shared_ptr<AutumnValue>>({})),
                    AutumnListType::getInstance()) {}

  std::string toString() const override {
    std::string result = "([";
    std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>> plist;
    try {
      plist = std::any_cast<
          std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>>>(value);
      result.reserve(2 + (plist->size() * 20) + (plist->size() - 1) * 2 + 3 + type->toString().length());
    } catch (const std::bad_any_cast &e) {
      std::cerr << "Error in toString(): bad_any_cast for 'value'."
                << std::endl;
      return "([Invalid List])";
    }

    if (!plist) {
      std::cerr << "Error in toString(): 'plist' is null." << std::endl;
      return "([Null List])";
    }

    for (size_t i = 0; i < plist->size(); i++) {
      auto &element = (*plist)[i];
      if (!element) {
        std::cerr << "WARNING: Element at index " << i << " is null."
                  << std::endl;
        result += "null";
      } else {
        try {
          result += element->toString();
        } catch (const std::exception &e) {
          std::cerr << "Error in element->toString() at index " << i << ": "
                    << e.what() << std::endl;
          result += "error";
        }
      }
      if (i != plist->size() - 1) {
        result += ", ";
      }
    }
    result += "] :" + (type ? type->toString() : "Unknown Type") + ")";
    return result;
  }
  bool isEqual(std::shared_ptr<AutumnValue> other) override;

  std::shared_ptr<AutumnValue> clone() override {
    return shared_from_this();
  }

  std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>> getValues() {
    try {
      return std::any_cast<
          std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>>>(value);
    } catch (std::bad_any_cast &e) {
      std::cerr << "AutumnList::getValues Error: " << e.what() << std::endl;
      throw e;
    }
  }

  void add(std::shared_ptr<AutumnValue> elem);
  bool isTruthy() override {
    // If value is nullptr, it's also false
    if (value.has_value() == false) {
      return false;
    }
    if (std::any_cast<
            std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>>>(value)
            ->size() == 0) {
      return false;
    }
    return true;
  }

  std::shared_ptr<AutumnValue> copy() override {
    auto pList = std::any_cast<
        std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>>>(value);
    std::vector<std::shared_ptr<AutumnValue>> newlist;
    newlist.reserve(pList->size());
    for (size_t i = 0; i < pList->size(); i++) {
      newlist.push_back((*pList)[i]->copy());
    }
    auto pNewList =
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(newlist);
    return std::make_shared<AutumnList>(pNewList);
  }
};

class AutumnNull final: public AutumnValue,
                   public std::enable_shared_from_this<AutumnNull> {
public:
  AutumnNull(int instId, std::any value)
      : AutumnValue(instId, value, AutumnUnknownType::getInstance()) {}
  AutumnNull(std::any value)
      : AutumnValue(value, AutumnUnknownType::getInstance()) {}
  std::string toString() const override { return "null"; }
  bool isEqual(std::shared_ptr<AutumnValue> other) override;
  bool isTruthy() override { return false; }
  std::shared_ptr<AutumnValue> clone() override {
    return shared_from_this();
  }

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnNull>(value);
  }
};

} // namespace Autumn
#endif
