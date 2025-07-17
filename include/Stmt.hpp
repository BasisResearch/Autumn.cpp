#ifndef STMT_H
#define STMT_H

#include "Expr.hpp"
#include "Token.hpp"
#include <any>
#include <memory>
#include <string>
#include <vector>

namespace Autumn {

class Block;
class Object;
class Expression;
class OnStmt;

class Stmt {
public:
  // Visitor interface
  class Visitor {
  public:
    virtual std::any visitBlockStmt(std::shared_ptr<Block> stmt) = 0;
    virtual std::any visitObjectStmt(std::shared_ptr<Object> stmt) = 0;
    virtual std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) = 0;
    virtual std::any visitOnStmtStmt(std::shared_ptr<OnStmt> stmt) = 0;
    virtual ~Visitor() = default;
  };

  virtual std::any accept(Visitor &visitor) = 0;
  virtual ~Stmt() = default;
};

class Block : public Stmt, public std::enable_shared_from_this<Block> {
public:
  Block(std::vector<std::shared_ptr<Stmt>> statements)
      : statements(statements) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitBlockStmt(shared_from_this());
  }

  const std::vector<std::shared_ptr<Stmt>> statements;
};

class Object : public Stmt, public std::enable_shared_from_this<Object> {
public:
  Object(Token name, std::vector<std::shared_ptr<Expr>> fields,
         std::shared_ptr<Expr> Cell)
      : name(name), fields(fields), Cell(Cell) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitObjectStmt(shared_from_this());
  }

  const Token name;
  const std::vector<std::shared_ptr<Expr>> fields;
  const std::shared_ptr<Expr> Cell;
};

class Expression : public Stmt,
                   public std::enable_shared_from_this<Expression> {
public:
  Expression(std::shared_ptr<Expr> expression) : expression(expression) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitExpressionStmt(shared_from_this());
  }

  const std::shared_ptr<Expr> expression;
};

class OnStmt : public Stmt, public std::enable_shared_from_this<OnStmt> {
public:
  OnStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Expr> expr)
      : condition(condition), expr(expr) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitOnStmtStmt(shared_from_this());
  }

  const std::shared_ptr<Expr> condition;
  const std::shared_ptr<Expr> expr;
};

} // namespace Autumn

#endif // STMT_H
