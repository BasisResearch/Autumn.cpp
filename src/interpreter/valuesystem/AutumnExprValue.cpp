#include "AutumnExprValue.hpp"
#include "AstPrinter.hpp"

namespace Autumn {
std::shared_ptr<AutumnExprType> AutumnExprType::instance = nullptr;

std::string AutumnExprValue::toString() const {
  try {
    return std::string("(" + AstPrinter().print(cexpr) + ": Expr)");
  } catch (Error &e) {
    throw Error(std::string("Error Printing Expr value: ") + e.what());
  }
}

bool AutumnExprValue::isEqual(std::shared_ptr<AutumnValue> other) {
  std::shared_ptr<AutumnExprValue> otherExpr =
      std::dynamic_pointer_cast<AutumnExprValue>(other);
  if (otherExpr == nullptr) {
    return false;
  }
  return cexpr == otherExpr->cexpr;
}
} // namespace Autumn
