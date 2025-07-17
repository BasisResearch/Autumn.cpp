#include "AutumnStdComponents.hpp"
#include "AstPrinter.hpp"
#include "AutumnCallable.hpp"
#include "AutumnCallableValue.hpp"
#include "AutumnConstructor.hpp"
#include "AutumnClass.hpp"
#include "AutumnExprValue.hpp"
#include "AutumnInstance.hpp"
#include "AutumnLambda.hpp"
#include "AutumnType.hpp"
#include "Expr.hpp"
#include "Parser.hpp"
#include "TokenType.hpp"
#include "sexpresso.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace Autumn {
std::shared_ptr<AutumnFuncType> AutumnFuncType::instance = nullptr;
// If class initializer is nullptr, then we take all values forwarded
// Then use them as init
// else if it is not null, it have to return a list of values

std::shared_ptr<AutumnClass> PositionClass = std::make_shared<AutumnClass>(
    std::string("Position"), std::vector<std::string>({"x", "y"}),
    std::vector<std::shared_ptr<AutumnType>>(
        {AutumnNumberType::getInstance(), AutumnNumberType::getInstance()}),
    nullptr, std::unordered_map<std::string, std::shared_ptr<AutumnMethod>>());

std::shared_ptr<AutumnClass> CellClass = std::make_shared<AutumnClass>(
    std::string("Cell"), std::vector<std::string>({"x", "y", "color"}),
    std::vector<std::shared_ptr<AutumnType>>({AutumnExprType::getInstance(),
                                              AutumnExprType::getInstance(),
                                              AutumnExprType::getInstance()}),
    nullptr, std::unordered_map<std::string, std::shared_ptr<AutumnMethod>>());

std::shared_ptr<AutumnClass> RenderedElemClass = std::make_shared<AutumnClass>(
    std::string("RenderedElem"),
    std::vector<std::string>({"position", "color"}),
    std::vector<std::shared_ptr<AutumnType>>(
        {PositionClass, AutumnStringType::getInstance()}),
    nullptr, std::unordered_map<std::string, std::shared_ptr<AutumnMethod>>());

std::shared_ptr<AutumnType> makeObjectClass(
    Interpreter &interpreter, std::string name,
    std::vector<std::pair<std::string, std::shared_ptr<AutumnType>>> &fields,
    std::shared_ptr<Expr> cexpr) {
  // The most important things are:
  // (1) list of fields
  // (2) list of elems
  std::vector<std::string> fieldNames;
  fieldNames.reserve(fields.size());
  std::vector<std::shared_ptr<AutumnType>> fieldTypes;
  fieldTypes.reserve(fields.size());

  for (auto &field : fields) {
    fieldNames.push_back(field.first);
    fieldTypes.push_back(field.second);
  }

  std::vector<Token> paramLists;

  std::vector<std::shared_ptr<Expr>> fieldsExprs;
  fieldsExprs.reserve(fieldNames.size() + 1);
  for (auto fieldName : fieldNames) {
    paramLists.push_back(Token(TokenType::IDENTIFIER, fieldName, nullptr, 0));
    fieldsExprs.push_back(std::make_shared<Variable>(
        Token(TokenType::IDENTIFIER, fieldName, nullptr, 0)));
  }
  std::vector<Token> paramListsWithPosition(paramLists);
  paramListsWithPosition.push_back(
      Token(TokenType::IDENTIFIER, "origin", nullptr, 0));

  AstPrinter printer;
  SExpParser tmpParser = SExpParser("");

  fieldsExprs.push_back(std::make_shared<Variable>(
      Token(TokenType::IDENTIFIER, "origin", nullptr, 0)));

  // Wrap cell expr in a list add
  try {
    auto assignExpr = std::make_shared<Assign>(
        Token(TokenType::IDENTIFIER, "allCells", nullptr, 0), cexpr);
    auto ifListTypeExpr = std::make_shared<Call>(
        std::make_shared<Variable>(
            Token(TokenType::IDENTIFIER, "isList", nullptr, 0)),
        std::vector<std::shared_ptr<Expr>>({std::make_shared<Variable>(
            Token(TokenType::IDENTIFIER, "allCells", nullptr, 0))}));
    auto ifListExpr = std::make_shared<IfExpr>(
        ifListTypeExpr,
        std::make_shared<Variable>(
            Token(TokenType::IDENTIFIER, "allCells", nullptr, 0)),
        std::make_shared<ListVarExpr>(
            std::vector<std::shared_ptr<Expr>>({std::make_shared<Variable>(
                Token(TokenType::IDENTIFIER, "allCells", nullptr, 0))})));

    auto cellExprBlock = std::make_shared<Let>(
        std::vector<std::shared_ptr<Expr>>({assignExpr, ifListExpr}));
    fieldsExprs.push_back(cellExprBlock);
  } catch (std::runtime_error &e) {
    throw std::runtime_error(
        "MakeObjectClass Error: Failed to create cell expr block: " +
        std::string(e.what()));
  }

  std::shared_ptr<ListVarExpr> allFieldsExpr =
      std::make_shared<ListVarExpr>(fieldsExprs);
  std::shared_ptr<Lambda> pLambda =
      std::make_shared<Lambda>(paramListsWithPosition, allFieldsExpr);

  std::vector<std::string> fullFieldNames(fieldNames);
  fullFieldNames.push_back(std::string("origin"));
  fullFieldNames.push_back(std::string("elems"));
  fieldTypes.push_back(PositionClass);
  fieldTypes.push_back(AutumnListType::getInstance(CellClass));
  // std::cerr<< ("Created initialization function: " +    
  //             std::any_cast<std::string>(pLambda->accept(printer)));

  std::shared_ptr<AutumnCallable> pCallable =
      std::make_shared<AutumnConstructor>(pLambda, nullptr);
  if (pCallable == nullptr) {
    throw std::runtime_error(
        "makeObjectClass: Failed to create callable for object class");
  }

  std::string renderString =
      "--> () (map (--> e (RenderedElem (Position (+ (.. origin x) ((.. e x))) "
      "(+ (.. origin y) ((.. e y)))) "
      "((.. e color))) ) elems)";
  auto renderLambda = sexpresso::parse(renderString);
  std::shared_ptr<Lambda> renderExpr =
      std::dynamic_pointer_cast<Lambda>(tmpParser.parseExpr(renderLambda));
  std::shared_ptr<AutumnCallable> renderCallable =
      std::make_shared<AutumnLambda>(renderExpr, nullptr);
  if (renderCallable == nullptr) {
    throw std::runtime_error(
        "makeObjectClass: Failed to create callable for render method");
  }
  std::shared_ptr<AutumnMethod> renderMethod = nullptr;
  try {
    renderMethod = std::make_shared<AutumnMethod>(renderCallable);
  } catch (std::runtime_error &e) {
    throw Error("Failed to create render method: " + std::string(e.what()));
  }
  std::unordered_map<std::string, std::shared_ptr<AutumnMethod>> methodsMap;
  methodsMap["render"] = renderMethod;
  return std::make_shared<AutumnClass>(name, fullFieldNames, fieldTypes,
                                       pCallable, methodsMap);
}
} // namespace Autumn
