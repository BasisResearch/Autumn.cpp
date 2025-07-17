#ifndef __AUTUMN_TYPE_HPP__
#define __AUTUMN_TYPE_HPP__

#include <memory>
#include <string>
#include <vector>

namespace Autumn {
class AutumnType {
public:
  virtual std::string toString() const = 0;
  virtual ~AutumnType() = default;
  virtual bool operator==(const AutumnType &other) const {
    return toString() == other.toString();
  }
};

class AutumnNumberType : public AutumnType {
  static std::shared_ptr<AutumnNumberType> instance;

public:
  static std::shared_ptr<AutumnNumberType> getInstance() {
    if (instance == nullptr) {
      instance = std::make_shared<AutumnNumberType>();
    }
    return instance;
  }
  std::string toString() const override { return "Number"; }
};

class AutumnStringType : public AutumnType {
  static std::shared_ptr<AutumnStringType> instance;

public:
  static std::shared_ptr<AutumnStringType> getInstance() {
    if (instance == nullptr) {
      instance = std::make_shared<AutumnStringType>();
    }
    return instance;
  }
  std::string toString() const override { return "String"; }
};

class AutumnBoolType : public AutumnType {
  static std::shared_ptr<AutumnBoolType> instance;

public:
  static std::shared_ptr<AutumnBoolType> getInstance() {
    if (instance == nullptr) {
      instance = std::make_shared<AutumnBoolType>();
    }
    return instance;
  }
  std::string toString() const override { return "Bool"; }
};

class AutumnUnknownType : public AutumnType {
  static std::shared_ptr<AutumnUnknownType> instance;

public:
  bool operator==(const AutumnType &other) const override { return false; }
  static std::shared_ptr<AutumnUnknownType> getInstance() {
    if (instance == nullptr) {
      instance = std::make_shared<AutumnUnknownType>();
    }
    return instance;
  }
  std::string toString() const override { return "Unknown"; }
};

class AutumnListType : public AutumnType {
private:
  std::shared_ptr<AutumnType> elementType;

public:
  AutumnListType(std::shared_ptr<AutumnType> elementType)
      : elementType(elementType) {}

  static std::shared_ptr<AutumnListType> getInstance() {
    static std::shared_ptr<AutumnListType> instance =
        std::make_shared<AutumnListType>(AutumnUnknownType::getInstance());
    return instance;
  }

  static std::shared_ptr<AutumnListType>
  getInstance(std::shared_ptr<AutumnType> elementType) {
    return std::make_shared<AutumnListType>(elementType);
  }

  bool operator==(const AutumnType &other) const override {
    const AutumnListType *otherListType =
        dynamic_cast<const AutumnListType *>(&other);
    if (otherListType == nullptr) {
      return false;
    }
    return elementType->toString() == otherListType->elementType->toString();
  }

  std::string toString() const override {
    return "List<" + elementType->toString() + ">";
  }

  std::shared_ptr<AutumnType> getElementType() const { return elementType; }
};

class AutumnMetaType : public AutumnType {
private:
  std::shared_ptr<AutumnType> type;

public:
  AutumnMetaType(std::shared_ptr<AutumnType> type) : type(type) {}

  static std::shared_ptr<AutumnMetaType> getInstance() {
    static std::shared_ptr<AutumnMetaType> instance =
        std::make_shared<AutumnMetaType>(AutumnUnknownType::getInstance());
    return instance;
  }

  static std::shared_ptr<AutumnMetaType>
  getInstance(std::shared_ptr<AutumnType> type) {
    return std::make_shared<AutumnMetaType>(type);
  }

  bool operator==(const AutumnType &other) const override {
    const AutumnMetaType *otherMetaType =
        dynamic_cast<const AutumnMetaType *>(&other);
    if (otherMetaType == nullptr) {
      return false;
    }
    return type->toString() == otherMetaType->type->toString();
  }

  std::string toString() const override {
    return "Meta<" + type->toString() + ">";
  }

  std::shared_ptr<AutumnType> getType() const { return type; }
};

} // namespace Autumn

#endif
