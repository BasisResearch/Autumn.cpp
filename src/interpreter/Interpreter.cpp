#include "VarCollector.hpp"
#include "Interpreter.hpp"
#include "AstPrinter.hpp"
#include "AutumnCallableValue.hpp"
#include "AutumnExprValue.hpp"
#include "AutumnInstance.hpp"
#include "AutumnLambda.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnType.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Expr.hpp"
#include "Parser.hpp"
#include "Stmt.hpp"
#include <any>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <cstdio>

static std::string readFile(const std::string &filename) {
  std::ifstream file(filename);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

namespace Autumn {
Interpreter::Interpreter() {
}

void Interpreter::init(std::string stdlib) {
  globals = std::make_shared<Environment>();
  environment = globals;
  // Reset onClauseCovered
  onClauseCovered.clear();
  onStmts.clear();
  // std::cout << globals->printAllDefinedVariables() << std::endl;

  std::shared_ptr<AutumnCallable> map = std::make_shared<Map>();
  try {
    globals->define("GRID_SIZE", std::make_shared<AutumnNumber>(16));
    globals->define("map", std::make_shared<AutumnCallableValue>(map));
    globals->define("concat", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<Concat>()));
    globals->define("filter", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<Filter>()));
    globals->define("foldl", std::make_shared<AutumnCallableValue>(
                                 std::make_shared<Foldl>()));
    globals->define("length", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<Length>()));
    globals->define("head", std::make_shared<AutumnCallableValue>(
                                std::make_shared<Head>()));
    globals->define("at", std::make_shared<AutumnCallableValue>(
                                std::make_shared<At>()));
    globals->define("tail", std::make_shared<AutumnCallableValue>(
                                std::make_shared<Tail>()));
    globals->define("renderAll", std::make_shared<AutumnCallableValue>(
                                     std::make_shared<RenderAll>()));
    globals->define("defined", std::make_shared<AutumnCallableValue>(
                                   std::make_shared<Defined>()));
    globals->define("arrayEqual", std::make_shared<AutumnCallableValue>(
                                   std::make_shared<ArrayEqual>()));
    globals->define("sqrt", std::make_shared<AutumnCallableValue>(
                                   std::make_shared<Sqrt>()));
  } catch (const Error &e) {
    if (getVerbose()) {
      std::cerr << "Error in initializing interpreter: " << e.what() << std::endl;
    }
  }
  globals->defineType("Bool", AutumnBoolType::getInstance());
  globals->defineType("String", AutumnStringType::getInstance());
  globals->defineType("Number", AutumnNumberType::getInstance());
  globals->defineType("Int", AutumnNumberType::getInstance());
  globals->defineType("Position", PositionClass);
  globals->defineType("Cell", CellClass);
  globals->defineType("RenderedElem", RenderedElemClass);
  try {
    globals->define("prev", std::make_shared<AutumnCallableValue>(
                                std::make_shared<Prev>()));
    globals->define(
        "any", std::make_shared<AutumnCallableValue>(std::make_shared<Any>()));
    globals->define("isList", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<IsList>()));
    globals->define("uniformChoice", std::make_shared<AutumnCallableValue>(
                                         std::make_shared<UniformChoice>()));
    globals->define("adjPositions", std::make_shared<AutumnCallableValue>(
                                        std::make_shared<AdjPositions>()));
    globals->define("addObj", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<AddObj>()));
    globals->define("isFreePos", std::make_shared<AutumnCallableValue>(
                                     std::make_shared<IsFreePos>()));
    globals->define("range", std::make_shared<AutumnCallableValue>(
                                 std::make_shared<Range>()));

    globals->define("print", std::make_shared<AutumnCallableValue>(
                                 std::make_shared<Print>()));
    globals->define("removeObj", std::make_shared<AutumnCallableValue>(
                                     std::make_shared<RemoveObj>()));
    globals->define("updateObj", std::make_shared<AutumnCallableValue>(
                                     std::make_shared<UpdateObj>()));
    globals->define("randomPositions",
                    std::make_shared<AutumnCallableValue>(
                        std::make_shared<RandomPositions>()));

    globals->define("allPositions", std::make_shared<AutumnCallableValue>(
                                        std::make_shared<AllPositions>()));
    globals->define("clicked", std::make_shared<AutumnCallableValue>(
                                   std::make_shared<Clicked>()));

    globals->define("left", std::make_shared<AutumnCallableValue>(
                                std::make_shared<LeftPressed>()));
    globals->define("right", std::make_shared<AutumnCallableValue>(
                                 std::make_shared<RightPressed>()));
    globals->define("up", std::make_shared<AutumnCallableValue>(
                              std::make_shared<UpPressed>()));
    globals->define("down", std::make_shared<AutumnCallableValue>(
                                std::make_shared<DownPressed>()));
    globals->define("isWithinBounds", std::make_shared<AutumnCallableValue>(
                                          std::make_shared<IsWithinBounds>()));

    globals->define("allObjs", std::make_shared<AutumnCallableValue>(
                                   std::make_shared<AllObjs>()));
    globals->define("rotate", std::make_shared<AutumnCallableValue>(
                                  std::make_shared<Rotate>()));
  } catch (const Error &e) {
    if (getVerbose()) {
      std::cerr << "Error in initializing interpreter: " << e.what() << std::endl;
    }
  }
  // read AutumnStdLib
  if (stdlib == "") {
    std::string autumnStdLib = readFile("autumnstdlib/stdlib.sexp");
    SExpParser parser(autumnStdLib);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parseStmt();
    for (const auto &stmt : stmts) {
      stmt->accept(*this);
    }
  } else {
    SExpParser parser(stdlib);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parseStmt();
    for (const auto &stmt : stmts) {
      stmt->accept(*this);
    }
  }
}

// If need to be evaluated, evaluate as normal
std::any Interpreter::visitLiteralExpr(std::shared_ptr<Literal> expr) {
  std::any value = expr->value;
  // Check if value is a number, a boolean, or a string
  try {
    return std::shared_ptr<AutumnValue>(
        std::make_shared<AutumnNumber>(std::any_cast<int>(value)));
  } catch (const std::bad_any_cast &) {
    try {
      return std::shared_ptr<AutumnValue>(
          std::make_shared<AutumnBool>(std::any_cast<bool>(value)));
    } catch (const std::bad_any_cast &) {
      try {
        return std::shared_ptr<AutumnValue>(
            std::make_shared<AutumnString>(std::any_cast<std::string>(value)));
      } catch (const std::bad_any_cast &) {
        throw Error("Unknown type of literal");
      }
    }
  }
}

std::any Interpreter::visitGroupingExpr(std::shared_ptr<Grouping> expr) {
  throw Error("This language does not supposed to have grouping :)");
}

std::any Interpreter::visitUnaryExpr(std::shared_ptr<Unary> expr) {
  std::shared_ptr<AutumnValue> right;
  try {
    right =
        std::any_cast<std::shared_ptr<AutumnValue>>(expr->right->accept(*this));
  } catch (const std::bad_any_cast &) {
    throw Error("Unary operator must be applied to a value");
  }
  switch (expr->op.type) {
  case TokenType::MINUS: {
    std::shared_ptr<AutumnNumber> rightNumber =
        std::dynamic_pointer_cast<AutumnNumber>(right);
    if (rightNumber == nullptr) {
      throw Error("Unary - must be applied to a number");
    }
    // std::cerr << "Unary -" << rightNumber->getNumber() << std::endl;
    return std::shared_ptr<AutumnValue>(
        std::make_shared<AutumnNumber>(-rightNumber->getNumber()));
  }
  case TokenType::PLUS: {
    std::shared_ptr<AutumnNumber> rightNumber =
        std::dynamic_pointer_cast<AutumnNumber>(right);
    if (rightNumber == nullptr) {
      throw Error("Unary + must be applied to a number");
    }
    // std::cerr << "Unary +" << rightNumber->getNumber() << std::endl;
    return std::shared_ptr<AutumnValue>(
        std::make_shared<AutumnNumber>(rightNumber->getNumber()));
  }
  case TokenType::BANG: {
    std::shared_ptr<AutumnBool> rightBool =
        std::dynamic_pointer_cast<AutumnBool>(right);
    if (rightBool == nullptr) {
      throw Error("Unary ! must be applied to a boolean");
    }
    auto res = std::make_shared<AutumnBool>(!rightBool->getBool());
    // std::cerr << "Unary !" << rightBool->getBool() << " = " << res->getBool()
    //          << std::endl;
    return std::dynamic_pointer_cast<AutumnValue>(res);
  }
  default:
    throw Error("Unknown unary operator");
  }
}

std::pair<std::shared_ptr<AutumnNumber>, std::shared_ptr<AutumnNumber>>
getBinaryNumber(std::shared_ptr<AutumnValue> left,
                std::shared_ptr<AutumnValue> right) {
  try {
    std::shared_ptr<AutumnNumber> leftNumber =
        std::dynamic_pointer_cast<AutumnNumber>(left);
    std::shared_ptr<AutumnNumber> rightNumber =
        std::dynamic_pointer_cast<AutumnNumber>(right);
    if (leftNumber == nullptr || rightNumber == nullptr) {
      throw Error("Binary operator must be applied to two numbers, instead got " +
                  left->toString() + " and " + right->toString());
    }
    return std::make_pair(leftNumber, rightNumber);
  } catch (const std::bad_any_cast &) {
    throw Error("Binary operator must be applied to two numbers, instead got " +
                left->toString() + " and " + right->toString());
  }
}

std::any Interpreter::visitBinaryExpr(std::shared_ptr<Binary> expr) {
  std::shared_ptr<AutumnValue> left, right;
  try {
    left =
        std::any_cast<std::shared_ptr<AutumnValue>>(expr->left->accept(*this));
    right =
        std::any_cast<std::shared_ptr<AutumnValue>>(expr->right->accept(*this));
  } catch (const std::bad_any_cast &e) {
    AstPrinter printer;
    throw Error("Binary operator must be applied to a value, instead got " +
                printer.print(expr->left) + " and " +
                printer.print(expr->right));
  }
  std::shared_ptr<AutumnValue> res;
  switch (expr->op.type) {
  case TokenType::PLUS: {
    try{
      std::pair<std::shared_ptr<AutumnNumber>, std::shared_ptr<AutumnNumber>>
          numbers = getBinaryNumber(left, right);
      res = std::make_shared<AutumnNumber>(numbers.first->getNumber() +
                                          numbers.second->getNumber());
    } catch (const Error &e) {
      throw Error("Binary + must be applied to two numbers, instead got " +
                  left->toString() + " and " + right->toString() + " op: " + expr->op.lexeme + " - " + e.what());
    }
    break;
  }
  case TokenType::MINUS: {
    try{
      std::pair<std::shared_ptr<AutumnNumber>, std::shared_ptr<AutumnNumber>>
          numbers = getBinaryNumber(left, right);
      res = std::make_shared<AutumnNumber>(numbers.first->getNumber() -
                                         numbers.second->getNumber());
    } catch (const Error &e) {
      throw Error("Binary - must be applied to two numbers, instead got " +
                  left->toString() + " and " + right->toString() + " op: " + expr->op.lexeme + " - " + e.what());
    }
    break;
  }
  case TokenType::STAR: {
    try{
      std::pair<std::shared_ptr<AutumnNumber>, std::shared_ptr<AutumnNumber>>
          numbers = getBinaryNumber(left, right);
      res = std::make_shared<AutumnNumber>(numbers.first->getNumber() *
                                         numbers.second->getNumber());
    } catch (const Error &e) {
      throw Error("Binary * must be applied to two numbers, instead got " +
                  left->toString() + " and " + right->toString() + " op: " + expr->op.lexeme + " - " + e.what());
    }
    break;
  }
  case TokenType::SLASH: {
    try{
      std::pair<std::shared_ptr<AutumnNumber>, std::shared_ptr<AutumnNumber>>
          numbers = getBinaryNumber(left, right);
      res = std::make_shared<AutumnNumber>(numbers.first->getNumber() /
                                         numbers.second->getNumber());
    } catch (const Error &e) {
      throw Error("Binary / must be applied to two numbers, instead got " +
                  left->toString() + " and " + right->toString() + " op: " + expr->op.lexeme + " - " + e.what());
    }
    break;
  }
  case TokenType::MODULO: {
    try{
      std::pair<std::shared_ptr<AutumnNumber>, std::shared_ptr<AutumnNumber>>
        numbers = getBinaryNumber(left, right);
      res = std::make_shared<AutumnNumber>(numbers.first->getNumber() %
                                         numbers.second->getNumber());
    } catch (const Error &e) {
      throw Error("Binary % must be applied to two numbers, instead got " +
                  left->toString() + " and " + right->toString() + " op: " + expr->op.lexeme + " - " + e.what());
    }
    break;
  }
  case TokenType::GREATER: {
    try{
      std::pair<std::shared_ptr<AutumnNumber>, std::shared_ptr<AutumnNumber>>
        numbers = getBinaryNumber(left, right);
      res = std::make_shared<AutumnBool>(numbers.first->getNumber() >
                                         numbers.second->getNumber());
    } catch (const Error &e) {
      throw Error("Binary > must be applied to two numbers, instead got " +
                  left->toString() + " and " + right->toString() + " op: " + expr->op.lexeme + " - " + e.what());
    }
    break;
  }
  case TokenType::GREATER_EQUAL: {
    try{
      std::pair<std::shared_ptr<AutumnNumber>, std::shared_ptr<AutumnNumber>>
        numbers = getBinaryNumber(left, right);
      res = std::make_shared<AutumnBool>(numbers.first->getNumber() >=
                                         numbers.second->getNumber());
    } catch (const Error &e) {
      throw Error("Binary >= must be applied to two numbers, instead got " +
                  left->toString() + " and " + right->toString() + " op: " + expr->op.lexeme + " - " + e.what());
    }
    break;
  }
  case TokenType::LESS: {
    try{
      std::pair<std::shared_ptr<AutumnNumber>, std::shared_ptr<AutumnNumber>>
        numbers = getBinaryNumber(left, right);
      res = std::make_shared<AutumnBool>(numbers.first->getNumber() <
                                       numbers.second->getNumber());
    } catch (const Error &e) {
      throw Error("Binary < must be applied to two numbers, instead got " +
                  left->toString() + " and " + right->toString() + " op: " + expr->op.lexeme + " - " + e.what());
    }
    break;
  }
  case TokenType::LESS_EQUAL: {
    try { 
      std::pair<std::shared_ptr<AutumnNumber>, std::shared_ptr<AutumnNumber>>
          numbers = getBinaryNumber(left, right);
      res = std::make_shared<AutumnBool>(numbers.first->getNumber() <=
                                         numbers.second->getNumber());
    } catch (const Error &e) {
      throw Error("Binary <= must be applied to two numbers, instead got " +
                  left->toString() + " and " + right->toString() + " op: " + expr->op.lexeme + " - " + e.what());
    }
    break;
  }
  case TokenType::EQUAL_EQUAL: {
    try {
      // std::cout << "Comparing " << left->toString() << " and "
      //          << right->toString() << std::endl;
      res = std::dynamic_pointer_cast<AutumnValue>(
          std::make_shared<AutumnBool>(left->isEqual(right)));
      return res;
      // std::cout << "Result: " << res->toString() << std::endl;
      break;
    } catch (const std::bad_any_cast &e) {
      throw Error("Cannot compare different types");
    }
  }
  case TokenType::BANG_EQUAL: {
    try {
      res = std::dynamic_pointer_cast<AutumnValue>(
          std::make_shared<AutumnBool>(!left->isEqual(right)));
      return res;
    } catch (const std::bad_any_cast &e) {
      throw Error("Cannot compare different types");
    }
  }
  default:
    throw Error("Unknown binary operator");
  }
  // std::cout << "Returning from binary: " << res->toString() << std::endl;
  return std::dynamic_pointer_cast<AutumnValue>(res);
}

std::any Interpreter::visitVariableExpr(std::shared_ptr<Variable> expr) {
  // std::cerr << "Visiting variable " << expr->name.lexeme << std::endl;
  try {
    std::shared_ptr<AutumnType> tv = std::any_cast<std::shared_ptr<AutumnType>>(
        environment->getTypeValue(expr->name));
    if (tv != nullptr) {
      // std::cerr << "Getting type value " << tv->toString() << std::endl;
      return tv;
    }
  } catch (const std::bad_any_cast &) {
    std::cerr << "Failing to get type having name "
              << environment->printAllTypeValues() << " - Must be a variable"
              << std::endl;
  }
  try {
    return environment->get(expr->name);
  } catch (const RuntimeError &e) {
    std::cerr << "Undefined variable '" + expr->name.lexeme + "'" +
                 environment->printAllDefinedVariables() + "\n" +
                 "Current expr: " + AstPrinter().print(expr);
    throw Error("Undefined variable '" + expr->name.lexeme + "'" +
                environment->printAllDefinedVariablesCrossStack() + "\n" +
                "Current expr: " + AstPrinter().print(expr));
  }
}

std::any Interpreter::visitAssignExpr(std::shared_ptr<Assign> expr) {
  // std::cerr << "Assigning " << expr->name.lexeme << std::endl;
  std::shared_ptr<InitNext> initNext =
      std::dynamic_pointer_cast<InitNext>(expr->value);
  if (initNext != nullptr) {
    initOrder.push_back(expr->name.lexeme);
    initMap[expr->name.lexeme] = initNext->initializer;
    nextMap[expr->name.lexeme] = initNext->nextExpr;
    return nullptr;
  }

  std::any retVal = expr->value->accept(*this);
  try {
    std::shared_ptr<AutumnValue> value =
        std::any_cast<std::shared_ptr<AutumnValue>>(retVal);
    auto isNameInGlobal = globals->isDefined(expr->name.lexeme);
    // Get type value
    std::shared_ptr<AutumnType> tv =
        environment->getAssignedType(expr->name.lexeme);
    if (tv != nullptr &&
        std::dynamic_pointer_cast<AutumnListType>(tv) == nullptr) {
      if (tv->toString() != value->getType()->toString()) {
        throw Error("Cannot assign value of type '" +
                    value->getType()->toString() + "' to variable of type '" +
                    tv->toString() + "' for variable '" + expr->name.lexeme +
                    "'.");
      }
    }
    environment->assign(expr->name, value);
    if (environment != globals && !isNameInGlobal) {
      if (globals->isDefined(expr->name.lexeme)) {
        throw Error("Variable assigned at local scope became defined at global "
                    "scope: " +
                    expr->name.lexeme); //+
                    // "\n Global: " + globals->printAllDefinedVariables() +
                    // "\n Local: " + environment->printAllDefinedVariables());
      }
    }
    // std::cerr << "Assigned " << expr->name.lexeme << " to " <<
    // value->toString()
    //           << std::endl;
    return value;
  } catch (const std::bad_any_cast &e) {
    throw Error("Assign only accept either value or InitNext " +
                AstPrinter().print(expr->value));
  }
}

std::any Interpreter::visitLogicalExpr(std::shared_ptr<Logical> expr) {
  std::shared_ptr<AutumnValue> left =
      std::any_cast<std::shared_ptr<AutumnValue>>(expr->left->accept(*this));
  auto callableLeft = std::dynamic_pointer_cast<AutumnCallableValue>(left);
  if (callableLeft != nullptr) {
    left = callableLeft->call(*this, {});
  }
  std::shared_ptr<AutumnValue> ret = nullptr;
  if (expr->op.type == TokenType::OR) {
    if (isTruthy(left)) {
      ret = std::dynamic_pointer_cast<AutumnValue>(
          std::make_shared<AutumnBool>(true));
      // std::cerr << "Returning from logical: " << ret->toString() <<
      // std::endl;
      return ret;
    }
  } else {
    if (!isTruthy(left)) {
      ret = std::dynamic_pointer_cast<AutumnValue>(
          std::make_shared<AutumnBool>(false));
      // std::cerr << "Returning from logical: " << ret->toString() <<
      // std::endl;
      return ret;
    }
  }
  try {
    auto rightValue =
        std::any_cast<std::shared_ptr<AutumnValue>>(expr->right->accept(*this));
    auto callableRight =
        std::dynamic_pointer_cast<AutumnCallableValue>(rightValue);
    if (callableRight != nullptr) {
      rightValue = callableRight->call(*this, {});
    }
    return std::dynamic_pointer_cast<AutumnValue>(
        std::make_shared<AutumnBool>(rightValue->isTruthy()));
  } catch (const std::bad_any_cast &e) {
    // std::cerr << "Logical operator must be applied to a value" << std::endl;
    throw Error("Logical operator must be applied to a value" +
                AstPrinter().print(expr->right));
  }
}

static std::shared_ptr<std::vector<std::shared_ptr<AutumnValue>>>
getAllArgs(std::shared_ptr<Call> expr, Interpreter &interpreter) {
  std::vector<std::shared_ptr<AutumnValue>> arguments;
  int i = 0;
  for (const auto &argument : expr->arguments) {
    try {
      arguments.push_back(std::any_cast<std::shared_ptr<AutumnValue>>(
          argument->accept(interpreter)));
    } catch (const std::bad_any_cast &e) {
      throw Error("Call arguments must be values");
    } catch (const Error &e) {
      throw Error("Call argument processing error while visiting: \n" +
                  AstPrinter().print(expr) + "\n Got: \n" + e.what());
    }
  }
  return std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(arguments);
}


std::any Interpreter::visitCallExpr(std::shared_ptr<Call> expr) {
  AstPrinter printer;
  std::any callee = expr->callee->accept(*this);

  try {
    std::shared_ptr<AutumnType> tv =
        std::any_cast<std::shared_ptr<AutumnType>>(callee);
    if (tv != nullptr) {
      // Check if this is a class
      std::shared_ptr<AutumnClass> cls =
          std::dynamic_pointer_cast<AutumnClass>(tv);
      if (cls != nullptr) {
        if (cls->name == "Cell") {
          std::shared_ptr<std::vector<std::string>> varExprs = std::make_shared<std::vector<std::string>>();
          VariableCollector collector;
          for (const auto &arg : expr->arguments) {
            auto subVarExprs = std::any_cast<std::shared_ptr<std::vector<std::string>>>(arg->accept(collector));
            varExprs->insert(varExprs->end(), subVarExprs->begin(), subVarExprs->end());
          }
          for (const auto &arg : expr->arguments) {
            auto subVarExprs = std::any_cast<std::shared_ptr<std::vector<std::string>>>(arg->accept(collector));
            varExprs->insert(varExprs->end(), subVarExprs->begin(), subVarExprs->end());
          }
          // std::cout << "VarExprs: " << varExprs->size() << std::endl;
          // for (const auto &varExpr : *varExprs) {
          //   std::cout << "VarExpr: " << varExpr << std::endl;
          // }
          std::vector<std::shared_ptr<AutumnValue>> cellArgs = {};
          // Create a closure
          EnvironmentPtr subEnv = std::make_shared<Environment>();
          // Copy everthing we can except the nearest instance enclosing.
          EnvironmentPtr skippingEnv = nullptr;
          // Find the nearest instance enclosing
          EnvironmentPtr currentEnv = environment;

          std::stack<EnvironmentPtr> envStack;
          while (currentEnv != nullptr) {
            envStack.push(currentEnv);
            currentEnv = currentEnv->getEnclosing();
          }

          while (!envStack.empty()) {
            EnvironmentPtr currentEnv = envStack.top();
            envStack.pop();
            if (currentEnv->getEnvironmentType() == EnvironmentType::INSTANCE
                && skippingEnv == nullptr) {
              skippingEnv = currentEnv;
            }
            else {
                // Copy everything over
                subEnv->selectiveCopy(currentEnv, *varExprs);
            }
          }
          EnvironmentPtr currEnv = this->getEnvironment();
          this->setEnvironment(subEnv);
          // TODO: Separate out/ remove outer attribute that are not evaluated
          for (const auto &arg : expr->arguments) {
            std::shared_ptr<AutumnExprValue> exprValue =
                std::make_shared<AutumnExprValue>(arg, subEnv);
            try {
              
              std::shared_ptr<AutumnValue> pValue =
                  std::any_cast<std::shared_ptr<AutumnValue>>(
                      arg->accept(*this));
              auto pNumber = std::dynamic_pointer_cast<AutumnNumber>(pValue);
              if (pNumber != nullptr) {
                cellArgs.push_back(std::make_shared<AutumnExprValue>(
                    std::make_shared<Literal>(pNumber->getNumber()), subEnv));
                continue;
              }
              cellArgs.push_back(exprValue);
            } catch (Error &e) {
              cellArgs.push_back(exprValue);
            }
          }
          this->setEnvironment(currEnv);
          return std::dynamic_pointer_cast<AutumnValue>(
              std::make_shared<AutumnInstance>(cls, cellArgs));
        }
        // First get all the argument
        auto arguments = getAllArgs(expr, *this);
        try {
          std::vector<std::shared_ptr<AutumnValue>> args = *arguments;
          if (cls->getInitializer() != nullptr) {
            std::shared_ptr<AutumnValue> pValue =
                cls->getInitializer()->call(*this, *arguments);
            std::shared_ptr<AutumnList> list =
                std::dynamic_pointer_cast<AutumnList>(pValue);
            if (list != nullptr) {
              args = std::vector<std::shared_ptr<AutumnValue>>(
                  list->getValues()->begin(), list->getValues()->end());
            }
          }
          auto retVal = std::dynamic_pointer_cast<AutumnValue>(
              std::make_shared<AutumnInstance>(cls, args));
          return retVal;
        } catch (const Error &e) {
          throw Error(
              std::string(
                  "DEBUG: Cannot instantiate class without initializer") +
              e.what());
        }
      }
    }
  } catch (const std::bad_any_cast &) {
  }
  std::shared_ptr<AutumnValue> calleeVal = nullptr;
  try {
    calleeVal = std::any_cast<std::shared_ptr<AutumnValue>>(callee);
    calleeVal = calleeVal->copy();
  } catch (const std::bad_any_cast &e) {
    throw Error(
        "Callee:: Can only call functions and classes or evaluate expresions");
  }
  try {
    std::shared_ptr<AutumnExprValue> exprValue =
        std::dynamic_pointer_cast<AutumnExprValue>(calleeVal);
    if (exprValue != nullptr) {
      // Print out this and the enclosing environment environment type
      std::string thisEnvType = this->getEnvironment()->getEnvironmentType() == EnvironmentType::INSTANCE ? "Instance" : "Global";
      std::string enclosingEnvType = this->getEnvironment()->getEnclosing() == nullptr ? "Global" : this->getEnvironment()->getEnclosing()->getEnvironmentType() == EnvironmentType::INSTANCE ? "Instance" : "Global";
      EnvironmentPtr subEnv = exprValue->getCenv();
      EnvironmentPtr currEnv = this->getEnvironment();
      subEnv->setEnclosing(currEnv);
      this->setEnvironment(subEnv);
      auto val = exprValue->getExpr()->accept(*this);
      subEnv->setEnclosing(nullptr);
      this->setEnvironment(currEnv);
      return val;
    }
  } catch (Error &e) {
    std::shared_ptr<AutumnExprValue> exprValue =
        std::dynamic_pointer_cast<AutumnExprValue>(calleeVal);
    throw Error(std::string("Error in visiting evaluation of expression: ") + AstPrinter().print(exprValue->getExpr()) + "\n" +
                e.what());
  }
  std::shared_ptr<AutumnCallableValue> callable =
      std::dynamic_pointer_cast<AutumnCallableValue>(calleeVal);
  if (callable == nullptr) {
    throw Error("Can only call functions and classes");
  }
  if (callable->toString() == "<native fn: Prev>" || callable->toString() == "<native fn: <native fn: Prev>>") {
    // Do not evaluate the arguments
    if (expr->arguments.size() != 1) {
      throw Error("[Prev] Prev() takes 1 argument");
    }
    std::shared_ptr<Variable> varExpr = std::dynamic_pointer_cast<Variable>(expr->arguments[0]);
    if (varExpr == nullptr) { // This is already a string
      std::shared_ptr<AutumnValue> retVal =
        std::any_cast<std::shared_ptr<AutumnValue>>(
            callable->call(*this, *getAllArgs(expr, *this)));
      return retVal;
    }
    std::shared_ptr<AutumnValue> varName = std::make_shared<AutumnString>(varExpr->name.lexeme);
    std::shared_ptr<AutumnValue> retVal =
        std::any_cast<std::shared_ptr<AutumnValue>>(
            callable->call(*this, {varName}));
    return retVal;
  }
  else {
    std::shared_ptr<AutumnValue> retVal =
        std::any_cast<std::shared_ptr<AutumnValue>>(
            callable->call(*this, *getAllArgs(expr, *this)));
    return retVal;
  }
}

std::any Interpreter::visitGetExpr(std::shared_ptr<Get> expr) {
  // std::cerr << "Visiting get: " << expr->name.lexeme << std::endl;
  std::shared_ptr<AutumnValue> object =
      std::any_cast<std::shared_ptr<AutumnValue>>(expr->object->accept(*this));
  if (object == nullptr) {
    throw Error("Error interpreting: " + AstPrinter().print(expr) +
                ": Cannot get property from null");
  }
  std::shared_ptr<AutumnInstance> instance =
      std::dynamic_pointer_cast<AutumnInstance>(object);
  if (instance == nullptr) {
    throw Error("Error interpreting: " + AstPrinter().print(expr) +
                "Only instances have properties, instead got " +
                object->getType()->toString());
  }
  std::shared_ptr<AutumnValue> value = instance->get(expr->name.lexeme);
  if (value == nullptr) {
    throw Error("Error interpreting: " + AstPrinter().print(expr) +
                "Undefined property '" + expr->name.lexeme + "'");
  }
  // std::cerr << "Returning from get: " << value->toString() << std::endl;
  return value;
}

std::any Interpreter::visitSetExpr(std::shared_ptr<Set> expr) {
  std::shared_ptr<AutumnValue> object =
      std::any_cast<std::shared_ptr<AutumnValue>>(expr->object->accept(*this));
  if (object == nullptr) {
    throw Error("Cannot set property on null");
  }
  std::shared_ptr<AutumnInstance> instance =
      std::dynamic_pointer_cast<AutumnInstance>(object);
  if (instance == nullptr) {
    throw Error("Only instances have fields");
  }
  std::shared_ptr<AutumnValue> value =
      std::any_cast<std::shared_ptr<AutumnValue>>(expr->value->accept(*this));
  instance->set(expr->name.lexeme, value);
  return value;
}

std::any Interpreter::visitLambdaExpr(std::shared_ptr<Lambda> expr) {
  AstPrinter printer;
  // std::cerr << "Visiting lambda: "
  //           << std::any_cast<std::string>(expr->accept(printer)) <<
  //           std::endl;
  std::shared_ptr<Environment> closure = environment; /// TODO: Fix if bug
  std::shared_ptr<AutumnLambda> lambda =
      std::make_shared<AutumnLambda>(expr, closure);
  if (lambda == nullptr) {
    throw Error("visitingLambdaExpr:: Error creating lambda");
  }
  try {
    std::shared_ptr<AutumnCallableValue> pCallableValue =
        std::make_shared<AutumnCallableValue>(lambda);
    // std::cerr << "Lambda created: " << pCallableValue->toString()
    //           << lambda->toString() << std::endl;
    return std::dynamic_pointer_cast<AutumnValue>(pCallableValue);
  } catch (const Error &e) {
    throw Error("visitingLambdaExpr:: Error creating callable value");
  }
}

std::any
Interpreter::visitTypeVariableExpr(std::shared_ptr<TypeVariable> expr) {
  return environment->getTypeValue(expr->name);
}

std::any Interpreter::visitTypeDeclExpr(std::shared_ptr<TypeDecl> expr) {
  std::shared_ptr<AutumnType> type = nullptr;
  try {
    type = std::any_cast<std::shared_ptr<AutumnType>>(
        expr->typeexpr->accept(*this));
  } catch (const std::bad_any_cast &e) {
    throw Error("TypeDecl must have a type" +
                AstPrinter().print(expr->typeexpr));
  }
  if (stateStack.size() != 0 && stateStack.top() == InterpretingState::OBJECT) {
    return std::make_pair<>(expr->name.lexeme, type);

  } else {
    // std::cerr << "TypeDecl " << expr->name.lexeme << " " << type->toString()
    //           << std::endl;
    environment->assignType(expr->name.lexeme, type);
    return nullptr;
  }
}

std::any
Interpreter::visitListTypeExprExpr(std::shared_ptr<ListTypeExpr> expr) {
  try {
    std::shared_ptr<AutumnType> ntype =
        std::any_cast<std::shared_ptr<AutumnType>>(
            expr->typeexpr->accept(*this));
    if (ntype == nullptr) {
      throw Error("List type must have a type, instead got" + AstPrinter().print(expr));
    }
    return std::shared_ptr<AutumnType>(AutumnListType::getInstance(ntype));
  } catch (const std::bad_any_cast &e) {
    throw Error("List type must have a type" + AstPrinter().print(expr));
  }
}

std::any Interpreter::visitListVarExprExpr(std::shared_ptr<ListVarExpr> expr) {
  try {
    auto pVarExprs =
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>();
    for (auto subexpr : expr->varExprs) {
      try {
        auto valueAny = subexpr->accept(*this);
        auto autumnValue =
            std::any_cast<std::shared_ptr<AutumnValue>>(valueAny);
        pVarExprs->push_back(autumnValue);
      } catch (const std::bad_any_cast &e) {
        throw Error("visitListVarExprExpr Failed To Interpret " +
                    AstPrinter().print(subexpr));
      }
    }
    auto plist = std::make_shared<AutumnList>(pVarExprs);
    return std::dynamic_pointer_cast<AutumnValue>(plist);
  } catch (const std::bad_any_cast &e) {
    throw Error("List must have values,got " + AstPrinter().print(expr));
  }
}

std::any Interpreter::visitIfExprExpr(std::shared_ptr<IfExpr> expr) {
  std::shared_ptr<AutumnValue> condition = nullptr;
  try {
    condition =
        std::any_cast<std::shared_ptr<AutumnValue>>(
            expr->condition->accept(*this));
    
  } catch (const std::bad_any_cast &e) {
    throw Error("If condition must be a boolean expression: " +
                AstPrinter().print(expr->condition) + " - " + e.what());
  }
  bool isCondTruthy = false;
  try {
    isCondTruthy = condition->isTruthy();
  } catch (const Error &e) {
    throw Error("If condition must be a boolean expression, instead got value: " +
                condition->toString() + " - " + e.what());
  }
  if (isCondTruthy) {
    try {
      return expr->thenBranch->accept(*this);
    } catch (const Error &e) {
      throw Error("If then branch error: " + AstPrinter().print(expr->thenBranch) +
      " - " + std::string(e.what()));
    }
  } else {
    try {
      return expr->elseBranch->accept(*this);
    } catch (const Error &e) {
      throw Error("If else branch error: " + AstPrinter().print(expr->elseBranch) +
      " - " + std::string(e.what()));
    }
  }
}

std::any Interpreter::visitLetExpr(std::shared_ptr<Let> expr) {
  auto subEnv = std::make_shared<Environment>(environment);
  setEnvironment(subEnv);
  std::shared_ptr<AutumnValue> value = nullptr;
  std::any ret;
  for (const auto &subexpr : expr->exprs) {
    ret = subexpr->accept(*this);
  }
  try {
    value = std::any_cast<std::shared_ptr<AutumnValue>>(ret);
  } catch (const std::bad_any_cast &e) {
    throw Error("Error visiting Let Expr- " + AstPrinter().print(expr) +
          "\nGot:" + e.what());
  }
  setEnvironment(subEnv->getEnclosing());
  return value;
}

std::any Interpreter::visitInitNextExpr(std::shared_ptr<InitNext> expr) {
  return nullptr;
}

std::any Interpreter::visitObjectStmt(std::shared_ptr<Object> stmt) {
  stateStack.push(InterpretingState::OBJECT);
  // std::cerr << "Visiting object stmt " << AstPrinter().print(stmt) <<
  // std::endl;
  std::vector<std::pair<std::string, std::shared_ptr<AutumnType>>> fields;
  for (const auto &field : stmt->fields) {
    fields.push_back(
        std::any_cast<std::pair<std::string, std::shared_ptr<AutumnType>>>(
            field->accept(*this)));
  }
  environment->defineType(
      stmt->name.lexeme,
      makeObjectClass(*this, stmt->name.lexeme, fields, stmt->Cell));
  stateStack.pop();
  // std::cerr << "Defined object " << stmt->name.lexeme << " with type "
  //           << std::any_cast<std::shared_ptr<AutumnType>>(
  //                  environment->getTypeValue(stmt->name))
  //                  ->toString()
  //           << std::endl;
  return nullptr;
}

std::any Interpreter::visitBlockStmt(std::shared_ptr<Block> stmt) {
  throw Error("Block statement is not allowed in this language");
}

std::any Interpreter::visitExpressionStmt(std::shared_ptr<Expression> stmt) {
  std::any value = stmt->expression->accept(*this);
  return nullptr;
}

std::any Interpreter::visitOnStmtStmt(std::shared_ptr<OnStmt> stmt) {
  onStmts.push_back(stmt);
  return nullptr;
}

void Interpreter::start(const std::vector<std::shared_ptr<Stmt>> &stmts,
                        std::string stdlib, std::string triggeringCondition, 
                        uint64_t randomSeed) {
  setRandomSeed(randomSeed);
  init(stdlib);
  environment->assign("SpecialConditionTriggered",
                  std::make_shared<AutumnBool>(false));
  if (triggeringCondition != "") {
    try {
      SExpParser parser(triggeringCondition);
      std::shared_ptr<Expr> expr =
          parser.parseExpr(sexpresso::parse(triggeringCondition));
      this->triggeringConditionExpr = expr;
    } catch (const Error &e) {
      throw Error(std::string("Failed to parse triggering condition: ") +
                  e.what());
    }
  }
  for (const auto &stmt : stmts) {
    stmt->accept(*this);
  }

  // Then start visiting initExpr
  for (const auto &[k, v] : initMap) {
    try {
      std::shared_ptr<AutumnValue> value =
          std::any_cast<std::shared_ptr<AutumnValue>>(v->accept(*this));
      std::string name = k;
      if (value == nullptr) {
        throw Error("Inititalizing: " + name + ": " +
                    "must have a value instead got null ptr");
      }
      // Fetch the corresponding type
      if (getVerbose()) {
        std::cerr << "Init " << name << ": " << std::endl
                << value->toString() << std::endl;
        // flush stderr
        std::cerr << std::endl;
      }
      std::shared_ptr<AutumnType> type = environment->getAssignedType(name);
      environment->define(name, value);
    } catch (const std::bad_any_cast &e) {
      throw Error("Init must have a value: " + AstPrinter().print(v));
    } catch (const RuntimeError &e) {
      if (getVerbose()) {
        std::cerr << "Warning:: Exception " << e.token.lexeme << std::endl;
      }
      throw Error("Init failed: " + std::string(e.what()));
    } catch (const Error &e) {
      throw Error("Init failed: " + std::string(e.what()));
    }
  }
  renderAll();
}

void Interpreter::tmpExecuteStmt(const std::shared_ptr<Stmt> &stmt) {
  try {
    cacheEnvironment(environment);
    std::shared_ptr<Expression> exprStmt = std::dynamic_pointer_cast<Expression>(stmt);
    std::shared_ptr<OnStmt> onStmt = std::dynamic_pointer_cast<OnStmt>(stmt);
    if (exprStmt != nullptr) {
    std::shared_ptr<Assign> assign = std::dynamic_pointer_cast<Assign>(exprStmt->expression);
    if (assign != nullptr) {
      // visit this
      visitAssignExpr(assign);
      std::shared_ptr<InitNext> initNext = std::dynamic_pointer_cast<InitNext>(assign->value);
      if (initNext != nullptr) {
        environment->define(assign->name.lexeme, 
        std::any_cast<std::shared_ptr<AutumnValue>>(initNext->initializer->accept(*this)));
      }
      return;
    }
  } else if (onStmt != nullptr) {
      onStmts.push_back(onStmt);
      return;
    }
    else{
    stmt->accept(*this);
    } 
  }catch (const Error &e) {
    throw Error("Error in tmpExecuteStmt: " + std::string(e.what()));
  }
}

void Interpreter::step() {
  // Copy previous globals
  // Delete previous environment
  auto old_prev_environment = prev_environment;
  if (prev_environment == nullptr) {
    prev_environment = std::make_shared<Environment>();
  }
  // Copy the stack
  std::stack<std::shared_ptr<Environment>> stack;
  auto current = environment;
  while (current != nullptr) {
    stack.push(current);
    current = current->getEnclosing();
  }
  std::shared_ptr<Environment> curr_ancestor = nullptr;
  while (!stack.empty()) {
    auto current = stack.top();
    auto new_env = current->copy(curr_ancestor);
    curr_ancestor = new_env;
    stack.pop();
  }
  prev_environment = curr_ancestor;
  environment->resetUpdateStates();

  // Execute triggeringCondition
  if (this->triggeringConditionExpr != nullptr) {
    std::shared_ptr<AutumnValue> condition =
        std::any_cast<std::shared_ptr<AutumnValue>>(
            this->triggeringConditionExpr->accept(*this));
    if (!condition->isTruthy()) {
      environment->assign("SpecialConditionTriggered",
                      std::make_shared<AutumnBool>(false));
    } else {
      environment->assign("SpecialConditionTriggered",
                      std::make_shared<AutumnBool>(true));
    }
  }
  // Execute onStmts
  std::vector<std::shared_ptr<std::string>> updated;
  AstPrinter printer;
  for (int i = 0; i < onStmts.size(); i++) {
    auto stmt = onStmts[i];
    std::shared_ptr<AutumnValue> condition = nullptr;
    std::shared_ptr<OnStmt> onStmt = std::dynamic_pointer_cast<OnStmt>(stmt);
    try {
      condition = std::any_cast<std::shared_ptr<AutumnValue>>(
          onStmt->condition->accept(*this));
    } catch (const std::bad_any_cast &e) {
      throw Error("On condition must be a value");
    }
    auto conditionBool = std::dynamic_pointer_cast<AutumnBool>(condition);
    auto callable = std::dynamic_pointer_cast<AutumnCallableValue>(condition);
    if (conditionBool == nullptr && callable == nullptr) {
      throw Error("On condition must be a boolean expression");
    }
    std::shared_ptr<AutumnBool> condEval;
    if (conditionBool != nullptr) {
      condEval = conditionBool;
    } else {
      std::vector<std::shared_ptr<AutumnValue>> args = {};
      condEval =
          std::dynamic_pointer_cast<AutumnBool>(callable->call(*this, args));
    }
    // std::cout << "Condition evaluated to " << condEval->getBool() <<
    // std::endl;
    if (condEval->isTruthy()) {
      onClauseCovered.insert(i);
      std::shared_ptr<AutumnValue> value =
          std::any_cast<std::shared_ptr<AutumnValue>>(
              onStmt->expr->accept(*this));
    }
  }
  for (auto kvPairs : nextMap) {
    std::string key = kvPairs.first;
    std::shared_ptr<Expr> nextExpr = kvPairs.second;
    if (environment->isUpdated(key)) {
      continue;
    }
    auto allDefineds = environment->getDefinedVariables();
    if (allDefineds.find(key) == allDefineds.end()) {
      continue;
    }
    std::shared_ptr<AutumnValue> value =
        std::any_cast<std::shared_ptr<AutumnValue>>(nextExpr->accept(*this));
    environment->assign(key, value);
  }
  this->state->reset();
}

static std::shared_ptr<AutumnList>
renderValue(Interpreter &interpreter, std::shared_ptr<AutumnValue> value) {
  auto pList = std::dynamic_pointer_cast<AutumnList>(value);
  auto pInstance = std::dynamic_pointer_cast<AutumnInstance>(value);
  if (pList != nullptr) {
    std::vector<std::shared_ptr<AutumnValue>> values;
    values.reserve(pList->getValues()->size()*3);
    for (auto &elem : *(pList->getValues())) {
      auto pRendered = renderValue(interpreter, elem);
      const auto& renderedValues = pRendered->getValues();
      values.insert(values.end(), renderedValues->begin(), renderedValues->end());
    }
    return std::make_shared<AutumnList>(
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(values));
  } else if (pInstance != nullptr) {
    if (pInstance->getClass()->findMethod("render") == nullptr) {
      return std::make_shared<AutumnList>(
          std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(
              std::vector<std::shared_ptr<AutumnValue>>()));
    }
    std::shared_ptr<AutumnCallableValue> render =
        std::dynamic_pointer_cast<AutumnCallableValue>(
            pInstance->get("render"));
    std::shared_ptr<AutumnValue> result = render->call(interpreter, {});
    return std::dynamic_pointer_cast<AutumnList>(result);
  } else {
    return std::make_shared<AutumnList>(
        std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(
            std::vector<std::shared_ptr<AutumnValue>>({})));
  }
}

std::string Interpreter::renderAll() {
  environment->clearOccupied();
  std::string result = "{";
  result.reserve(10000);
  auto allDefineds = environment->getDefinedVariables();
  auto vars = environment->getDefinitionOrder();
  auto visited = std::unordered_set<std::string>();
  visited.reserve(initOrder.size() + vars.size());
  // copy vars to initOrder
  auto renderOrder = std::vector<std::string>();
  renderOrder.reserve(initOrder.size() + vars.size());
  for (auto &elem : initOrder) {
    if (visited.find(elem) == visited.end()) {
      renderOrder.push_back(elem);
      visited.insert(elem);
    }
  }
  for (auto &elem : vars) {
    if (visited.find(elem) == visited.end()) {
      renderOrder.push_back(elem);
      visited.insert(elem);
    }
  }
  auto pAllElems = std::make_shared<AutumnList>();
  pAllElems->getValues()->reserve(10000);
  static constexpr const char* ELEMENTS_TEMPLATE = "\"%s\": [";
  static constexpr const char* POSITION_TEMPLATE = 
    "{\"position\": {\"x\": %d, \"y\": %d}, \"color\": %s}, ";
  for (auto k : renderOrder) {
    auto pList = renderValue(*this, allDefineds[k]);
    if (pList->getValues()->size() == 0) {
      continue;
    }
    char buffer[256];
    snprintf(buffer, sizeof(buffer), ELEMENTS_TEMPLATE, k.c_str());
    result += buffer;
    for (auto &elem : *(pList->getValues())) {
      auto elemInstance = std::dynamic_pointer_cast<AutumnInstance>(elem);
      pAllElems->getValues()->push_back(elemInstance);
      auto pos = std::dynamic_pointer_cast<AutumnInstance>(
          elemInstance->get("position"));
      auto x =
          std::dynamic_pointer_cast<AutumnNumber>(pos->get("x"))->getNumber();
      auto y =
          std::dynamic_pointer_cast<AutumnNumber>(pos->get("y"))->getNumber();
      environment->occupyPosition(x, y);
      auto color =
          std::dynamic_pointer_cast<AutumnString>(elemInstance->get("color"))
              ->getString();
      char buffer[256];
      snprintf(buffer, sizeof(buffer), POSITION_TEMPLATE, 
              x, y, color.c_str());
      result += buffer;
    }
    if (pList->getValues()->size() > 0) {
      result = result.substr(0, result.size() - 2);
    }
    result += "], ";
  }
  result += std::string("\"GRID_SIZE\": ") +
            std::to_string(std::dynamic_pointer_cast<AutumnNumber>(
                               environment->get("GRID_SIZE"))
                               ->getNumber());
  result += "}";
  this->getEnvironment()->assign("cacheRendered", pAllElems);
  return result;
}

std::string Interpreter::getBackground() {
  auto var = globals->get("background");
  if (var == nullptr) {
    return "black";
  }
  return std::dynamic_pointer_cast<AutumnString>(var)->getString().substr(1, std::dynamic_pointer_cast<AutumnString>(var)->getString().size() - 2);
}

int Interpreter::getFrameRate() {
  auto var = globals->get("FRAME_RATE");
  if (var == nullptr) {
    return 16;
  }
  return std::dynamic_pointer_cast<AutumnNumber>(var)->getNumber();
}


std::string Interpreter::evaluateToString(std::string expr) {
  SExpParser parser(expr);
  auto parsedExpr = parser.parseExpr(sexpresso::parse(expr));
  auto result = std::any_cast<std::shared_ptr<AutumnValue>>(parsedExpr->accept(*this));
  if (result == nullptr) {
    return "NULL";
  }
  return result->toString();
}

} // namespace Autumn
