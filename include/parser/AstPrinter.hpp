#ifndef _AUTUMN_ASTPRINTER_HPP_
#define _AUTUMN_ASTPRINTER_HPP_
#include "Error.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <any>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Autumn {
class AstPrinter : public Expr::Visitor, public Stmt::Visitor {
  std::string parenthesize(const std::string &name,
                           const std::vector<std::string> &expr_strs) {
    std::string result = "(" + name;
    for (const auto &expr_s : expr_strs) {
      result += " ";
      result += expr_s;
    }
    result += ")";
    return result;
  }

public:
  std::string print(const std::shared_ptr<Expr> &expr) {
    return std::any_cast<std::string>(expr->accept(*this));
  }

  std::string print(const std::shared_ptr<Stmt> &stmt) {
    return std::any_cast<std::string>(stmt->accept(*this));
  }

  std::any visitAssignExpr(std::shared_ptr<Assign> expr) override {
    try {
      std::string valueString =
          std::any_cast<std::string>(expr->value->accept(*this));
      return parenthesize("<assign> " + expr->name.lexeme, {valueString});
    } catch (const std::bad_any_cast &e) {
      throw Error("Assign: Unknown type");
    }
  }

  std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override {
    std::string leftString =
        std::any_cast<std::string>(expr->left->accept(*this));
    std::string rightString =
        std::any_cast<std::string>(expr->right->accept(*this));
    return parenthesize(expr->op.lexeme, {leftString, rightString});
  }

  std::any visitCallExpr(std::shared_ptr<Call> expr) override {
    std::string calleeString =
        std::any_cast<std::string>(expr->callee->accept(*this));
    std::vector<std::string> arguments;
    for (const auto &argument : expr->arguments) {
      arguments.push_back(std::any_cast<std::string>(argument->accept(*this)));
    }
    return parenthesize("<call> " + calleeString, arguments);
  }

  std::any visitGetExpr(std::shared_ptr<Get> expr) override {
    std::string objectString =
        std::any_cast<std::string>(expr->object->accept(*this));
    return parenthesize("<get>" + expr->name.lexeme, {objectString});
  }

  std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override {
    // Check if literal is string, boolean or int
    try {
      std::any_cast<int>(expr->value);
      return "(NUMBER " + std::to_string(std::any_cast<int>(expr->value)) + ")";
    } catch (const std::bad_any_cast &e) {
      try {
        std::any_cast<bool>(expr->value);
        return std::any_cast<bool>(expr->value) ? std::string("(BOOL true)")
                                                : std::string("(BOOL false)");
      } catch (const std::bad_any_cast &e) {
        try {
          std::any_cast<std::string>(expr->value);
          return "(STRING " + std::any_cast<std::string>(expr->value) + ")";
        } catch (const std::bad_any_cast &e) {
          throw Error("Unknown literal type");
        }
      }
    }
    return std::to_string(std::any_cast<int>(expr->value));
  }

  std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override {
    std::string expressionString =
        std::any_cast<std::string>(expr->expression->accept(*this));
    return parenthesize("group", {expressionString});
  }

  std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override {
    std::string rightString =
        std::any_cast<std::string>(expr->right->accept(*this));
    return parenthesize(expr->op.lexeme, {rightString});
  }

  std::any visitVariableExpr(std::shared_ptr<Variable> expr) override {
    return "(var " + expr->name.lexeme + ")";
  }

  std::any visitLogicalExpr(std::shared_ptr<Logical> expr) override {
    std::string leftString =
        std::any_cast<std::string>(expr->left->accept(*this));
    std::string rightString =
        std::any_cast<std::string>(expr->right->accept(*this));
    return parenthesize(expr->op.lexeme, {leftString, rightString});
  }

  std::any visitSetExpr(std::shared_ptr<Set> expr) override {
    std::string objectString =
        std::any_cast<std::string>(expr->object->accept(*this));
    std::string valueString =
        std::any_cast<std::string>(expr->value->accept(*this));
    return parenthesize("<set>" + expr->name.lexeme,
                        {objectString, valueString});
  }

  std::any visitLambdaExpr(std::shared_ptr<Lambda> expr) override {
    std::vector<std::string> params;
    for (const auto &param : expr->params) {
      params.push_back(param.lexeme);
    }
    std::string paramsString = "(";
    for (const auto &param : params) {
      paramsString += param + " ";
    }
    paramsString += ")";
    std::string rightString =
        std::any_cast<std::string>(expr->right->accept(*this));
    return parenthesize("lambda", {paramsString, rightString});
  }

  std::any visitTypeVariableExpr(std::shared_ptr<TypeVariable> expr) override {
    std::string nameString = expr->name.lexeme;
    return "(<type> " + nameString + ")";
  }

  std::any visitTypeDeclExpr(std::shared_ptr<TypeDecl> expr) override {
    std::string nameString = expr->name.lexeme;
    std::string typeString =
        std::any_cast<std::string>(expr->typeexpr->accept(*this));
    return "(<typeDecl> " + nameString + " " + typeString + ")";
  }

  std::any visitListTypeExprExpr(std::shared_ptr<ListTypeExpr> expr) override {
    std::string typeString =
        std::any_cast<std::string>(expr->typeexpr->accept(*this));
    return "(<List> " + typeString + ")";
  }

  std::any visitListVarExprExpr(std::shared_ptr<ListVarExpr> expr) override {
    std::vector<std::string> varExprs;
    for (const auto &varExpr : expr->varExprs) {
      varExprs.push_back(std::any_cast<std::string>(varExpr->accept(*this)));
    }
    std::string result = "[";
    for (const auto &varExpr : varExprs) {
      result += varExpr;
    }
    result += "]";
    return result;
  }

  std::any visitIfExprExpr(std::shared_ptr<IfExpr> expr) override {
    std::string conditionString =
        std::any_cast<std::string>(expr->condition->accept(*this));
    std::string thenBranchString =
        std::any_cast<std::string>(expr->thenBranch->accept(*this));
    std::string elseBranchString =
        std::any_cast<std::string>(expr->elseBranch->accept(*this));
    return parenthesize("if",
                        {conditionString, thenBranchString, elseBranchString});
  }

  std::any visitLetExpr(std::shared_ptr<Let> expr) override {
    std::vector<std::string> subExprs;
    for (const auto &subexpr : expr->exprs) {
      subExprs.push_back(std::any_cast<std::string>(subexpr->accept(*this)));
    }
    std::string result = "{";
    for (const auto &varExpr : subExprs) {
      result += varExpr;
    }
    result += "}";
    return result;
  }

  std::any visitInitNextExpr(std::shared_ptr<InitNext> expr) override {
    try {
      std::string initializerString =
          std::any_cast<std::string>(expr->initializer->accept(*this));
      std::string nextString =
          std::any_cast<std::string>(expr->nextExpr->accept(*this));
      return parenthesize("initnext", {initializerString, nextString});
    } catch (const std::bad_any_cast &e) {
      throw Error("InitNext: Unknown type");
    }
  }

  std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) override {
    try {
      return std::any_cast<std::string>(stmt->expression->accept(*this));
    } catch (const std::bad_any_cast &e) {
      throw Error("visitExpressionStmt: bad_any_cast");
    }
  }

  std::any visitBlockStmt(std::shared_ptr<Block> stmt) override {
    std::vector<std::string> statements;
    for (const auto &statement : stmt->statements) {
      statements.push_back(
          std::any_cast<std::string>(statement->accept(*this)));
    }
    std::string result = "";
    for (const auto &statement : statements) {
      result += statement;
    }
    return "{" + result + "}";
  }

  std::any visitObjectStmt(std::shared_ptr<Object> stmt) override {
    std::vector<std::string> fields;
    for (const auto &field : stmt->fields) {
      fields.push_back(std::any_cast<std::string>(field->accept(*this)));
    }
    std::string result = "<field>:(";
    for (const auto &field : fields) {
      result += field;
    }
    result += ")";
    std::string cellString =
        std::any_cast<std::string>(stmt->Cell->accept(*this));
    return "(<object> " + stmt->name.lexeme + " " + result + " " + cellString +
           ")";
  }

  std::any visitOnStmtStmt(std::shared_ptr<OnStmt> stmt) override {
    std::string conditionString =
        std::any_cast<std::string>(stmt->condition->accept(*this));
    std::string exprString =
        std::any_cast<std::string>(stmt->expr->accept(*this));
    return "(on " + conditionString + ": " + exprString + ")";
  }
};
} // namespace Autumn

#endif
