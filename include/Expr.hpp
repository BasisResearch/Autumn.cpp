#ifndef EXPR_H
#define EXPR_H

#include "Token.hpp"
#include <any>
#include <memory>
#include <string>
#include <vector>

namespace Autumn {

class Assign;
class Binary;
class Call;
class Get;
class Grouping;
class Literal;
class Logical;
class Set;
class Unary;
class Lambda;
class Variable;
class TypeVariable;
class TypeDecl;
class ListTypeExpr;
class ListVarExpr;
class IfExpr;
class Let;
class InitNext;

class Expr {
public:
  // Visitor interface
  class Visitor {
  public:
    virtual std::any visitAssignExpr(std::shared_ptr<Assign> stmt) = 0;
    virtual std::any visitBinaryExpr(std::shared_ptr<Binary> stmt) = 0;
    virtual std::any visitCallExpr(std::shared_ptr<Call> stmt) = 0;
    virtual std::any visitGetExpr(std::shared_ptr<Get> stmt) = 0;
    virtual std::any visitGroupingExpr(std::shared_ptr<Grouping> stmt) = 0;
    virtual std::any visitLiteralExpr(std::shared_ptr<Literal> stmt) = 0;
    virtual std::any visitLogicalExpr(std::shared_ptr<Logical> stmt) = 0;
    virtual std::any visitSetExpr(std::shared_ptr<Set> stmt) = 0;
    virtual std::any visitUnaryExpr(std::shared_ptr<Unary> stmt) = 0;
    virtual std::any visitLambdaExpr(std::shared_ptr<Lambda> stmt) = 0;
    virtual std::any visitVariableExpr(std::shared_ptr<Variable> stmt) = 0;
    virtual std::any
    visitTypeVariableExpr(std::shared_ptr<TypeVariable> stmt) = 0;
    virtual std::any visitTypeDeclExpr(std::shared_ptr<TypeDecl> stmt) = 0;
    virtual std::any
    visitListTypeExprExpr(std::shared_ptr<ListTypeExpr> stmt) = 0;
    virtual std::any
    visitListVarExprExpr(std::shared_ptr<ListVarExpr> stmt) = 0;
    virtual std::any visitIfExprExpr(std::shared_ptr<IfExpr> stmt) = 0;
    virtual std::any visitLetExpr(std::shared_ptr<Let> stmt) = 0;
    virtual std::any visitInitNextExpr(std::shared_ptr<InitNext> stmt) = 0;
    virtual ~Visitor() = default;
  };

  virtual std::any accept(Visitor &visitor) = 0;
  virtual ~Expr() = default;
};

class Assign : public Expr, public std::enable_shared_from_this<Assign> {
public:
  Assign(Token name, std::shared_ptr<Expr> value) : name(name), value(value) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitAssignExpr(shared_from_this());
  }

  const Token name;
  const std::shared_ptr<Expr> value;
};

class Binary : public Expr, public std::enable_shared_from_this<Binary> {
public:
  Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left(left), op(op), right(right) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitBinaryExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;
};

class Call : public Expr, public std::enable_shared_from_this<Call> {
public:
  Call(std::shared_ptr<Expr> callee,
       std::vector<std::shared_ptr<Expr>> arguments)
      : callee(callee), arguments(arguments) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitCallExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> callee;
  const std::vector<std::shared_ptr<Expr>> arguments;
};

class Get : public Expr, public std::enable_shared_from_this<Get> {
public:
  Get(std::shared_ptr<Expr> object, Token name) : object(object), name(name) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitGetExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> object;
  const Token name;
};

class Grouping : public Expr, public std::enable_shared_from_this<Grouping> {
public:
  Grouping(std::shared_ptr<Expr> expression) : expression(expression) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitGroupingExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> expression;
};

class Literal : public Expr, public std::enable_shared_from_this<Literal> {
public:
  Literal(std::any value) : value(value) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitLiteralExpr(shared_from_this());
  }

  const std::any value;
};

class Logical : public Expr, public std::enable_shared_from_this<Logical> {
public:
  Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left(left), op(op), right(right) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitLogicalExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;
};

class Set : public Expr, public std::enable_shared_from_this<Set> {
public:
  Set(std::shared_ptr<Expr> object, Token name, std::shared_ptr<Expr> value)
      : object(object), name(name), value(value) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitSetExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> object;
  const Token name;
  const std::shared_ptr<Expr> value;
};

class Unary : public Expr, public std::enable_shared_from_this<Unary> {
public:
  Unary(Token op, std::shared_ptr<Expr> right) : op(op), right(right) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitUnaryExpr(shared_from_this());
  }

  const Token op;
  const std::shared_ptr<Expr> right;
};

class Lambda : public Expr, public std::enable_shared_from_this<Lambda> {
public:
  Lambda(std::vector<Token> params, std::shared_ptr<Expr> right)
      : params(params), right(right) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitLambdaExpr(shared_from_this());
  }

  const std::vector<Token> params;
  const std::shared_ptr<Expr> right;
};

class Variable : public Expr, public std::enable_shared_from_this<Variable> {
public:
  Variable(Token name) : name(name) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitVariableExpr(shared_from_this());
  }

  const Token name;
};

class TypeVariable : public Expr,
                     public std::enable_shared_from_this<TypeVariable> {
public:
  TypeVariable(Token name) : name(name) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitTypeVariableExpr(shared_from_this());
  }

  const Token name;
};

class TypeDecl : public Expr, public std::enable_shared_from_this<TypeDecl> {
public:
  TypeDecl(Token name, std::shared_ptr<Expr> typeexpr)
      : name(name), typeexpr(typeexpr) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitTypeDeclExpr(shared_from_this());
  }

  const Token name;
  const std::shared_ptr<Expr> typeexpr;
};

class ListTypeExpr : public Expr,
                     public std::enable_shared_from_this<ListTypeExpr> {
public:
  ListTypeExpr(std::shared_ptr<Expr> typeexpr) : typeexpr(typeexpr) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitListTypeExprExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> typeexpr;
};

class ListVarExpr : public Expr,
                    public std::enable_shared_from_this<ListVarExpr> {
public:
  ListVarExpr(std::vector<std::shared_ptr<Expr>> varExprs)
      : varExprs(varExprs) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitListVarExprExpr(shared_from_this());
  }

  const std::vector<std::shared_ptr<Expr>> varExprs;
};

class IfExpr : public Expr, public std::enable_shared_from_this<IfExpr> {
public:
  IfExpr(std::shared_ptr<Expr> condition, std::shared_ptr<Expr> thenBranch,
         std::shared_ptr<Expr> elseBranch)
      : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitIfExprExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> condition;
  const std::shared_ptr<Expr> thenBranch;
  const std::shared_ptr<Expr> elseBranch;
};

class Let : public Expr, public std::enable_shared_from_this<Let> {
public:
  Let(std::vector<std::shared_ptr<Expr>> exprs) : exprs(exprs) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitLetExpr(shared_from_this());
  }

  const std::vector<std::shared_ptr<Expr>> exprs;
};

class InitNext : public Expr, public std::enable_shared_from_this<InitNext> {
public:
  InitNext(std::shared_ptr<Expr> initializer, std::shared_ptr<Expr> nextExpr)
      : initializer(initializer), nextExpr(nextExpr) {}

  std::any accept(Visitor &visitor) override {
    return visitor.visitInitNextExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> initializer;
  const std::shared_ptr<Expr> nextExpr;
};

} // namespace Autumn

#endif // EXPR_H
