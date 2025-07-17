#ifndef AUTUMN_EXPR_VALUE_HPP
#define AUTUMN_EXPR_VALUE_HPP

#include "AutumnType.hpp"
#include "AutumnValue.hpp"
#include "Expr.hpp"
#include "Environment.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Autumn {

class AutumnExprType : public AutumnType {

public:
  static std::shared_ptr<AutumnExprType> instance;

  std::string toString() const override { return std::string("Expr"); }

  static std::shared_ptr<AutumnExprType> getInstance() {
    if (instance == nullptr) {
      instance = std::make_shared<AutumnExprType>();
    }
    return instance;
  }
};

class AutumnExprValue : public AutumnValue, 
                       public std::enable_shared_from_this<AutumnExprValue> {
private:
  std::shared_ptr<Expr> cexpr;
  std::shared_ptr<Environment> cenv;

public:
  AutumnExprValue(int instId, std::shared_ptr<Expr> cexpr, std::shared_ptr<Environment> cenv)
      : AutumnValue(instId, cexpr, AutumnExprType::getInstance()),
        cexpr(cexpr), cenv(cenv) {}

  AutumnExprValue(std::shared_ptr<Expr> cexpr, std::shared_ptr<Environment> cenv)
      : AutumnValue(cexpr, AutumnExprType::getInstance()), cexpr(cexpr), cenv(cenv) {}

  std::string toString() const override;
  bool isEqual(std::shared_ptr<AutumnValue> other) override;
  std::shared_ptr<AutumnValue> clone() override {
    return shared_from_this();
  }

  std::shared_ptr<Expr> getExpr() { return cexpr; }
  std::shared_ptr<Environment> getCenv() { return cenv; }

  std::shared_ptr<AutumnValue> copy() override {
    return std::make_shared<AutumnExprValue>(cexpr, cenv);
  }
};

} // namespace Autumn

#endif
