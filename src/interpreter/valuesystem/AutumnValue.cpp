#include "AutumnValue.hpp"
#include "AutumnInstance.hpp"

namespace Autumn {
int AutumnValue::instCount = 0;

bool AutumnNumber::isEqual(std::shared_ptr<AutumnValue> other) {
  if (other == nullptr) {
    return false;
  }
  if (std::dynamic_pointer_cast<AutumnNumber>(other) == nullptr) {
    if (std::dynamic_pointer_cast<AutumnBool>(other) == nullptr) {
      return false;
    }
    return getNumber() ==
           (std::dynamic_pointer_cast<AutumnBool>(other)->isTruthy() ? 1 : 0);
  }

  return getNumber() ==
         std::dynamic_pointer_cast<AutumnNumber>(other)->getNumber();
}

bool AutumnString::isEqual(std::shared_ptr<AutumnValue> other) {
  if (other == nullptr) {
    return false;
  }
  if (std::dynamic_pointer_cast<AutumnString>(other) == nullptr) {
    return false;
  }
  return getString() ==
         std::dynamic_pointer_cast<AutumnString>(other)->getString();
}

bool AutumnBool::isEqual(std::shared_ptr<AutumnValue> other) {
  if (other == nullptr) {
    return false;
  }
  if (std::dynamic_pointer_cast<AutumnBool>(other) == nullptr) {
    return false;
  }
  return isTruthy() == std::dynamic_pointer_cast<AutumnBool>(other)->isTruthy();
}

bool AutumnList::isEqual(std::shared_ptr<AutumnValue> other) {
  if (other == nullptr) {
    return false;
  }
  if (std::dynamic_pointer_cast<AutumnList>(other) == nullptr) {
    return false;
  }
  return toString() == std::dynamic_pointer_cast<AutumnList>(other)->toString();
}

bool AutumnNull::isEqual(std::shared_ptr<AutumnValue> other) {
  if (other == nullptr) {
    return false;
  }
  if (std::dynamic_pointer_cast<AutumnNull>(other) == nullptr) {
    return false;
  }
  return true;
}

void AutumnList::add(std::shared_ptr<AutumnValue> elem) {
  try {
    auto plist = std::any_cast<
        std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>>>(value);
    if (plist == nullptr) {
      throw Error("AutumnList::add Error: List is null");
    }
    if (elem == nullptr) {
      throw Error("AutumnList::add Error: Element is null");
    }
    auto instElem = std::dynamic_pointer_cast<AutumnInstance>(elem);
    if (elem->getType()->toString() !=
            std::dynamic_pointer_cast<AutumnListType>(type)
                ->getElementType()
                ->toString() &&
        std::dynamic_pointer_cast<AutumnUnknownType>(
            std::dynamic_pointer_cast<AutumnListType>(type)
                ->getElementType()) == nullptr &&
        instElem == nullptr) {
      throw Error("List element type mismatch: got " +
                  elem->getType()->toString() + " expected " +
                  std::dynamic_pointer_cast<AutumnListType>(type)
                      ->getElementType()
                      ->toString());
    }
    this->type = AutumnListType::getInstance(elem->getType());
    plist->push_back(elem);
  } catch (std::bad_any_cast &e) {
    throw Error("AutumnList::add Error: " + std::string(e.what()));
  }
}

} // namespace Autumn
