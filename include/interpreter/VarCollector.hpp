#ifndef __AUTUMN_VARIABLE_COLLECTOR_HPP__
#define __AUTUMN_VARIABLE_COLLECTOR_HPP__

#include <vector>
#include <any>
#include <memory>
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Error.hpp"

namespace Autumn {

class VariableCollector: public Expr::Visitor, public Stmt::Visitor {

    public:
    // Visiting functions for expressions
  std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override{
    return std::make_shared<std::vector<std::string>>();
  }
  std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override{
    throw Error("This language does not supposed to have grouping :)");
  }
  std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override{
    return expr->right->accept(*this);
  }
  std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override{
    std::shared_ptr<std::vector<std::string>> left = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->left->accept(*this));
    std::shared_ptr<std::vector<std::string>> right = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->right->accept(*this));
    left->insert(left->end(), right->begin(), right->end());
    return left;
  }
  std::any visitVariableExpr(std::shared_ptr<Variable> expr) override{
    return std::shared_ptr<std::vector<std::string>>(std::make_shared<std::vector<std::string>>(1, expr->name.lexeme));
  }
  std::any visitAssignExpr(std::shared_ptr<Assign> expr) override{
    return expr->value->accept(*this);
  }
  std::any visitLogicalExpr(std::shared_ptr<Logical> expr) override{
    std::shared_ptr<std::vector<std::string>> left = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->left->accept(*this));
    std::shared_ptr<std::vector<std::string>> right = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->right->accept(*this));
    left->insert(left->end(), right->begin(), right->end());
    return left;
  }
  std::any visitCallExpr(std::shared_ptr<Call> expr) override{
    std::shared_ptr<std::vector<std::string>> callee = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->callee->accept(*this));
    for (auto argument : expr->arguments) {
      auto subVarExprs = std::any_cast<std::shared_ptr<std::vector<std::string>>>(argument->accept(*this));
      callee->insert(callee->end(), subVarExprs->begin(), subVarExprs->end());
    }
    return callee;
  }
  std::any visitGetExpr(std::shared_ptr<Get> expr) override {
    auto result = std::shared_ptr<std::vector<std::string>>(std::make_shared<std::vector<std::string>>());
    result->push_back(expr->name.lexeme);
    auto objectVarExprs = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->object->accept(*this));
    result->insert(result->end(), objectVarExprs->begin(), objectVarExprs->end());
    return result;
  }
  std::any visitSetExpr(std::shared_ptr<Set> expr) override{
    return expr->value->accept(*this);
  }
  std::any visitLambdaExpr(std::shared_ptr<Lambda> expr) override{
    return expr->right->accept(*this);
  }
  std::any visitTypeVariableExpr(std::shared_ptr<TypeVariable> expr) override{
    return std::make_shared<std::vector<std::string>>();
  }
  std::any visitTypeDeclExpr(std::shared_ptr<TypeDecl> expr) override{
    return std::make_shared<std::vector<std::string>>();
  }
  std::any visitListTypeExprExpr(std::shared_ptr<ListTypeExpr> expr) override{
    return std::make_shared<std::vector<std::string>>();
  }
  std::any visitListVarExprExpr(std::shared_ptr<ListVarExpr> expr) override{
    std::shared_ptr<std::vector<std::string>> varExprs = std::make_shared<std::vector<std::string>>();
    varExprs->reserve(expr->varExprs.size());
    for (auto varExpr : expr->varExprs) {
        auto subVarExprs = std::any_cast<std::shared_ptr<std::vector<std::string>>>(varExpr->accept(*this));
        varExprs->insert(varExprs->end(), subVarExprs->begin(), subVarExprs->end());
    }
    return varExprs;
  }
  std::any visitIfExprExpr(std::shared_ptr<IfExpr> expr) override{
    std::shared_ptr<std::vector<std::string>> condition = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->condition->accept(*this));
    std::shared_ptr<std::vector<std::string>> thenBranch = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->thenBranch->accept(*this));
    std::shared_ptr<std::vector<std::string>> elseBranch = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->elseBranch->accept(*this));
    condition->insert(condition->end(), thenBranch->begin(), thenBranch->end());
    condition->insert(condition->end(), elseBranch->begin(), elseBranch->end());
    return condition;
  }
  std::any visitLetExpr(std::shared_ptr<Let> expr) override{
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    result->reserve(expr->exprs.size());
    for (auto expr : expr->exprs) {
      auto subVarExprs = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->accept(*this));
      result->insert(result->end(), subVarExprs->begin(), subVarExprs->end());
    }
    return result;
  }

  // Visiting functions for statements
  std::any visitBlockStmt(std::shared_ptr<Block> stmt) override {
    throw Error("Block statement is not allowed in this language");
  }
  std::any visitObjectStmt(std::shared_ptr<Object> stmt) override{
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    result->reserve(stmt->fields.size());
    for (auto field : stmt->fields) {
      auto subVarExprs = std::any_cast<std::shared_ptr<std::vector<std::string>>>(field->accept(*this));
      result->insert(result->end(), subVarExprs->begin(), subVarExprs->end());
    }
    return result;
  }
  std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) override{
    return stmt->expression->accept(*this);
  }
  std::any visitOnStmtStmt(std::shared_ptr<OnStmt> stmt) override{
    std::shared_ptr<std::vector<std::string>> condition = std::any_cast<std::shared_ptr<std::vector<std::string>>>(stmt->condition->accept(*this));
    std::shared_ptr<std::vector<std::string>> expr = std::any_cast<std::shared_ptr<std::vector<std::string>>>(stmt->expr->accept(*this));
    condition->insert(condition->end(), expr->begin(), expr->end());
    return condition;
  }

  std::any visitInitNextExpr(std::shared_ptr<InitNext> expr) override{
    std::shared_ptr<std::vector<std::string>> initializer = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->initializer->accept(*this));
    std::shared_ptr<std::vector<std::string>> nextExpr = std::any_cast<std::shared_ptr<std::vector<std::string>>>(expr->nextExpr->accept(*this));
    initializer->insert(initializer->end(), nextExpr->begin(), nextExpr->end());
    return initializer;
  }
};

}
#endif // !__AUTUMN_VARIABLE_COLLECTOR_HPP__