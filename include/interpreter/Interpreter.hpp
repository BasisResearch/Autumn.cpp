#ifndef _AUTUMN_INTERPRETER_HPP_
#define _AUTUMN_INTERPRETER_HPP_
#include "Environment.hpp"
#include "Error.hpp"
#include "Expr.hpp"
#include "State.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include <any>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "RandomGenerator.hpp"

namespace Autumn {
struct SharedExprPtrHash {
  std::size_t operator()(const std::shared_ptr<Expr> &expr) const noexcept {
    return std::hash<const Expr *>()(expr.get());
  }
};

// Define the equality function for std::shared_ptr<Expr>
struct SharedExprPtrEqual {
  bool operator()(const std::shared_ptr<Expr> &lhs,
                  const std::shared_ptr<Expr> &rhs) const noexcept {
    return lhs.get() == rhs.get();
  }
};

class Interpreter : public Expr::Visitor, public Stmt::Visitor {
  std::unordered_map<std::shared_ptr<Expr>, int, SharedExprPtrHash,
                     SharedExprPtrEqual>
      locals;

  const std::shared_ptr<State> state = std::make_shared<State>();
  std::stack<std::shared_ptr<Environment>> tmpEnvStack;

  std::shared_ptr<Environment> globals = std::make_shared<Environment>();
  std::shared_ptr<Environment> environment = globals;
  std::shared_ptr<Environment> prev_environment = nullptr;
  const std::vector<std::shared_ptr<Stmt>> stmts;


  std::shared_ptr<RandomGenerator> randomGen;
  bool verbose = false;


  bool isTruthy(std::shared_ptr<AutumnValue> object) {
    return object->isTruthy();
  }



  std::any evaluate(std::shared_ptr<Expr> expr) { return expr->accept(*this); }


  bool isEqual(std::shared_ptr<AutumnValue> a, std::shared_ptr<AutumnValue> b) {
    if (a == nullptr && b == nullptr) {
      return true;
    }
    if (a == nullptr) {
      return false;
    }
    return a->isEqual(b);
  }

  void init(std::string = "");

  enum InterpretingState { NONE, OBJECT };
  std::stack<InterpretingState> stateStack;
  std::vector<std::string> initOrder;

  std::unordered_map<std::string, std::shared_ptr<Expr>> initMap;
  std::unordered_map<std::string, std::shared_ptr<Expr>> nextMap;

  std::vector<std::shared_ptr<Stmt>> onStmts;
  std::shared_ptr<Expr> triggeringConditionExpr = nullptr;
  std::unordered_set<int> onClauseCovered;

public:
  std::string evaluateToString(std::string expr) ;
  int getOnClauseCount() { return onStmts.size(); }
  int getCoveredOnClauseCount() { return onClauseCovered.size(); }

  void start(const std::vector<std::shared_ptr<Stmt>> &stmts, std::string = "",
             std::string = "", uint64_t randomSeed = 0);
  void step();
  void reloadCode(const std::vector<std::shared_ptr<Stmt>> &stmts, std::string stdlib, std::string triggeringCondition);
  void tmpExecuteStmt(const std::shared_ptr<Stmt> &stmt);
  std::shared_ptr<State> getState() { return state; }

  std::any lookUpVariable(Token name, std::shared_ptr<Expr> expr) {
    auto distance = locals.find(expr);
    if (distance != locals.end()) {
      return environment->getAt(distance->second, name.lexeme);
    } else {
      return globals->get(name);
    }
  }

  void resolve(std::shared_ptr<Expr> expr, int depth) { locals[expr] = depth; }

  const std::shared_ptr<Environment> &getGlobals() { return globals; }
  const std::shared_ptr<Environment> &getPrevEnvironment() { return prev_environment; }

  std::shared_ptr<Environment> getEnvironment() { return environment; }
  Interpreter();

  bool getTriggerState() {
    // Check if it has been defined in global
    if (globals->get("SpecialConditionTriggered") != nullptr) {
      return std::dynamic_pointer_cast<AutumnBool>(
                 globals->get("SpecialConditionTriggered"))
          ->isTruthy();
    } else {
      return false;
    }
  }

  // Visiting functions for expressions
  std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override;
  std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override;
  std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override;
  std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override;
  std::any visitVariableExpr(std::shared_ptr<Variable> expr) override;
  std::any visitAssignExpr(std::shared_ptr<Assign> expr) override;
  std::any visitLogicalExpr(std::shared_ptr<Logical> expr) override;
  std::any visitCallExpr(std::shared_ptr<Call> expr) override;
  std::any visitGetExpr(std::shared_ptr<Get> expr) override;
  std::any visitSetExpr(std::shared_ptr<Set> expr) override;
  std::any visitLambdaExpr(std::shared_ptr<Lambda> expr) override;
  std::any visitTypeVariableExpr(std::shared_ptr<TypeVariable> expr) override;
  std::any visitTypeDeclExpr(std::shared_ptr<TypeDecl> expr) override;
  std::any visitListTypeExprExpr(std::shared_ptr<ListTypeExpr> expr) override;
  std::any visitListVarExprExpr(std::shared_ptr<ListVarExpr> expr) override;
  std::any visitIfExprExpr(std::shared_ptr<IfExpr> expr) override;
  std::any visitLetExpr(std::shared_ptr<Let> expr) override;
  std::any visitInitNextExpr(std::shared_ptr<InitNext> expr) override;

  // Visiting functions for statements
  std::any visitBlockStmt(std::shared_ptr<Block> stmt) override;
  std::any visitObjectStmt(std::shared_ptr<Object> stmt) override;
  std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) override;
  std::any visitOnStmtStmt(std::shared_ptr<OnStmt> stmt) override;

  void setEnvironment(std::shared_ptr<Environment> env) { environment = env; }

  std::string renderAll();
  std::string getEnvironmentString() {
    return environment->toJson();
  }

  void cacheEnvironment(std::shared_ptr<Environment> env) {
    tmpEnvStack.push(environment);
    environment = env->copy(environment->getEnclosing());
  }

  void restoreEnvironment() { environment = tmpEnvStack.top(); tmpEnvStack.pop(); }

  void setVerbose(bool verbose) { this->verbose = verbose; }
  bool getVerbose() { return verbose; }

  void setRandomSeed(uint64_t seed) {
        if (!randomGen) {
            randomGen = std::make_shared<RandomGenerator>(seed);
        } else {
            randomGen->setSeed(seed);
        }
  }
    
  std::shared_ptr<RandomGenerator> getRandomGenerator() const {
      return randomGen;
  }

  std::string getBackground();
  int getFrameRate();
};
} // namespace Autumn
#endif
