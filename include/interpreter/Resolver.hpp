#ifndef _AUTUMN_RESOLVER_HPP_
#define _AUTUMN_RESOLVER_HPP_
#include "Environment.hpp"
#include "Error.hpp"
#include "Expr.hpp"
#include "Interpreter.hpp"
#include "Stmt.hpp"
#include <memory>
#include <optional>
#include <stack>
#include <unordered_map>

namespace Autumn {
class Resolver : public Expr::Visitor, public Stmt::Visitor {
  // Not sure if there is anything to track beside from the let stmt
private:
  const std::shared_ptr<Interpreter> interpreter;
  std::shared_ptr<Environment> environment;
  std::stack<std::unordered_map<std::string, bool>> scopes;

  enum class FunctionType {
    NONE,
    FUNCTION,
  };

  enum class ClassType {
    NONE,
    OBJECT,
  };

  FunctionType currentFunction = FunctionType::NONE;

  void resolve(const std::vector<std::shared_ptr<Stmt>> &statements) {
    for (const auto &stmt : statements) {
      resolve(stmt);
    }
  }

  void resolve(const std::shared_ptr<Stmt> &stmt) { stmt->accept(*this); }

  void resolve(const std::shared_ptr<Expr> &expr) { expr->accept(*this); }

  void beginScope() { scopes.push({}); }
  void endScope() { scopes.pop(); }

  void declare(const Token &name) {
    if (scopes.empty()) {
      return;
    }

    auto &scope = scopes.top();
    if (scope.find(name.lexeme) != scope.end()) {
      throw RuntimeError(
          name, "Variable with this name already declared in this scope.");
    }
    scope[name.lexeme] = false;
  }

  void define(const Token &name) {
    if (scopes.empty()) {
      return;
    }

    auto &scope = scopes.top();
    scope[name.lexeme] = true;
  }

  bool resolveLocal(const std::shared_ptr<Expr> expr, Token name) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
      if (scopes.top().find(name.lexeme) != scopes.top().end()) {
        interpreter->resolve(expr, scopes.size() - 1 - i);
        return true;
      }
    }
    return false;
  }

  std::any resolveLambda(Lambda *lambda, FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;
    beginScope();
    for (const auto &param : lambda->params) {
      declare(param);
      define(param);
    }
    resolve(lambda->right);
    endScope();
    currentFunction = enclosingFunction;
    return nullptr;
  }

public:
  Resolver(std::shared_ptr<Interpreter> interpreter)
      : interpreter(interpreter), environment(std::make_shared<Environment>()) {
  }

  std::any visitBlockStmt(Block *stmt) {
    beginScope();
    resolve(stmt->statements);
    endScope();
  }

  std::any visitExpressionStmt(Expression *stmt) {
    resolve(stmt->expression);
    return nullptr;
  }

  std::any visitObjectStmt(Object *stmt) {
    declare(stmt->name);
    define(stmt->name);
    return nullptr;
  }

  std::any visitOnStmtStmt(OnStmt *stmt) { return nullptr; }

  std::any visitVariableExpr(Variable *expr) {
    resolveLocal(std::make_shared<Variable>(expr), expr->name);
    return nullptr;
  }

  std::any visitAssignExpr(Assign *expr) {
    resolve(expr->value);
    bool found = resolveLocal(std::make_shared<Variable>(expr), expr->name);
    if (!found) {
      declare(expr->name);
      define(expr->name);
    }
    return nullptr;
  }

  std::any visitBinaryExpr(Binary *expr) {
    resolve(expr->left);
    resolve(expr->right);
    return nullptr;
  }

  std::any visitCallExpr(Call *expr) {
    resolve(expr->callee);
    for (const auto &argument : expr->arguments) {
      resolve(argument);
    }
    return nullptr;
  }

  std::any visitGetExpr(Get *expr) {
    resolve(expr->object);
    return nullptr;
  }

  std::any visitGroupingExpr(Grouping *expr) {
    resolve(expr->expression);
    return nullptr;
  }

  std::any visitLiteralExpr(Literal *expr) { return nullptr; }

  std::any visitLogicalExpr(Logical *expr) {
    resolve(expr->left);
    resolve(expr->right);
    return nullptr;
  }

  std::any visitSetExpr(Set *expr) {
    resolve(expr->value);
    resolve(expr->object);
    return nullptr;
  }

  std::any visitUnaryExpr(Unary *expr) {
    resolve(expr->right);
    return nullptr;
  }

  std::any visitLambdaExpr(Lambda *expr) {
    resolveLambda(expr, FunctionType::FUNCTION);
    return nullptr;
  }

  std::any visitTypeVariableExpr(TypeVariable *expr) { return nullptr; }

  std::any visitTypeDeclExpr(TypeDecl *expr) {
    if (!scopes.empty() &&
        scopes.top().find(expr->name.lexeme) != scopes.top().end() &&
        scopes.top()[expr->name.lexeme] == false) {
      throw RuntimeError(expr->name, "Cannot read local variable in its own "
                                     "initializer.");
    }
    // cast and resolve
    std::shared_ptr<Expr> sharedExpr = std::make_shared<Variable>(expr);
    resolveLocal(sharedExpr, expr->name);
    return nullptr;
    return nullptr;
  }

  std::any visitListTypeExprExpr(ListTypeExpr *expr) {
    expr->typeexpr->accept(*this);
  }

  std::any visitListVarExprExpr(ListVarExpr *expr) {
    for (auto vExpr : expr->varExprs) {
      vExpr->accept(*this);
    }
  }

  std::any visitCellExprExpr(CellExpr *expr) {
    return nullptr;
  } /// DO NOT RESOLVE THIS FOR NOW, It will be evaluated on the initnext

  std::any visitIfExprExpr(IfExpr *expr) {
    resolve(expr->condition);
    resolve(expr->thenBranch);
    if (expr->elseBranch != nullptr) {
      resolve(expr->elseBranch);
    }
    return nullptr;
  }

  std::any visitInitNextExpr(InitNext *expr) { return nullptr; } /// Global!
};
} // namespace Autumn

#endif
