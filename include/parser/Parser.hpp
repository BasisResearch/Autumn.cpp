#ifndef __AUTUMN_SEXP_PARSER_HPP__
#define __AUTUMN_SEXP_PARSER_HPP__

#include "AutumnStdLib.hpp"
#include "Expr.hpp"
#include "Lexer.hpp"
#include "Stmt.hpp"
#include "Token.hpp"
#include "TokenType.hpp"
#include "sexpresso.hpp"
#include <memory>
#include <vector>

namespace Autumn {

class SExpParser {
private:
  const std::string &source;
  int current = 0;

  bool isBinaryOp(const TokenType &op) {

    return op == TokenType::PLUS || op == TokenType::MINUS ||
           op == TokenType::STAR || op == TokenType::SLASH ||
           op == TokenType::EQUAL_EQUAL || op == TokenType::BANG_EQUAL ||
           op == TokenType::GREATER || op == TokenType::GREATER_EQUAL ||
           op == TokenType::LESS || op == TokenType::LESS_EQUAL ||
           op == TokenType::MODULO;
  }

  bool isUnaryOp(const TokenType &op) {
    return op == TokenType::MINUS || op == TokenType::PLUS ||
           op == TokenType::BANG;
  }

  bool isLogicalOp(const TokenType &op) {
    return op == TokenType::AND || op == TokenType::OR;
  }

  bool isNumber(const std::string &str) {
    for (char c : str) {
      if (!std::isdigit(c)) {
        return false;
      }
    }
    return true;
  }

  bool startsWithNumber(const std::string &str) { return std::isdigit(str[0]); }

public:
  SExpParser(const std::string &source) : source(source) {}

  std::shared_ptr<Expr> parseTypeExpr(std::shared_ptr<sexpresso::Sexp> sexp,
                                      int line = -1) {
    auto head = sexp;
    if (!sexp->isString()) {
      head = sexp->getChild(0);
    }
    auto tok = Lexer(head->getString()).scanTokens()[0];
    if (head->isString()) {
      if (tok.type != TokenType::IDENTIFIER) {
        throw std::runtime_error(
            "ParseError: Type must be an identifier, instead got " +
            tok.lexeme + "of type " + TokenTypeToString(tok.type) +
            " at line " + std::to_string(line));
      }
      if (tok.lexeme == "updateObj") {
        throw std::runtime_error("ParseError: UpdateObj is not a type");
      } else if (tok.lexeme == "Union") {
        throw std::runtime_error(
            "ParseError: This language does not allow Type Union ");
      }
      if (tok.lexeme == "List") {
        return std::make_shared<ListTypeExpr>(parseTypeExpr(sexp->getChild(1)));
      }
      return std::make_shared<TypeVariable>(Token(
          TokenType::IDENTIFIER, head->getString(), head->getString(), line));
    } else {
      throw std::runtime_error("ParseError: Invalid type expression");
    }
  }

  std::shared_ptr<Expr> parseTypeDecl(std::shared_ptr<sexpresso::Sexp> sexp,
                                      int line = -1) {
    auto name = sexp->getChild(1);
    auto tok = Lexer(name->getString()).scanTokens()[0];
    if (tok.type != TokenType::IDENTIFIER) {
      throw std::runtime_error(
          "ParseError: Field/Var name must be an identifier,instead got " +
          tok.lexeme);
    }
    return std::make_shared<TypeDecl>(
        Token(tok.type, tok.lexeme, tok.literal, line),
        parseTypeExpr(sexp->getChild(2)));
  }

  std::shared_ptr<Expr> parseIfExpr(std::shared_ptr<sexpresso::Sexp> sexp,
                                    int line = -1) {
    return std::make_shared<IfExpr>(parseExpr(sexp->getChild(1), line),
                                    parseExpr(sexp->getChild(3), line),
                                    parseExpr(sexp->getChild(5), line));
  }

  std::shared_ptr<Expr> parseBinaryExpr(std::shared_ptr<sexpresso::Sexp> sexp,
                                        int line = -1) {
    std::string tstring = sexp->getChild(0)->toString();
    TokenType ttype = getTokenType(sexp->getChild(0)->toString());
    return std::make_shared<Binary>(parseExpr(sexp->getChild(1)),
                                    Token(ttype, tstring, nullptr, line),
                                    parseExpr(sexp->getChild(2)));
  }

  std::shared_ptr<Expr> parseLogicalExpr(std::shared_ptr<sexpresso::Sexp> sexp,
                                         int line = -1) {
    std::string tstring = sexp->getChild(0)->getString();
    TokenType ttype = getTokenType(sexp->getChild(0)->getString());
    return std::make_shared<Logical>(parseExpr(sexp->getChild(1)),
                                     Token(ttype, tstring, nullptr, line),
                                     parseExpr(sexp->getChild(2)));
  }

  std::shared_ptr<Expr> parseInitNextExpr(std::shared_ptr<sexpresso::Sexp> sexp,
                                          int line = -1) {
    return std::make_shared<InitNext>(parseExpr(sexp->getChild(1)),
                                      parseExpr(sexp->getChild(2)));
  }

  std::shared_ptr<Expr> parseAssignExpr(std::shared_ptr<sexpresso::Sexp> sexp,
                                        int line = -1) {
    std::shared_ptr<Expr> lhs = parseExpr(sexp->getChild(1), line);
    std::shared_ptr<Expr> rhs = parseExpr(sexp->getChild(2), line);
    std::shared_ptr<Variable> var = std::dynamic_pointer_cast<Variable>(lhs);
    std::shared_ptr<Get> get = std::dynamic_pointer_cast<Get>(lhs);
    if (var == nullptr && get == nullptr) {
      throw std::runtime_error("ParseError: Left hand side must be a variable");
    }
    if (var != nullptr) {
      return std::make_shared<Assign>(var->name, rhs);
    } else {
      return std::make_shared<Set>(get->object, get->name, rhs);
    }
  }

  std::shared_ptr<Expr> parseGetExpr(std::shared_ptr<sexpresso::Sexp> sexp,
                                     int line) {
    return std::make_shared<Get>(parseExpr(sexp->getChild(1), line),
                                 Token(TokenType::IDENTIFIER,
                                       sexp->getChild(2)->getString(),
                                       sexp->getChild(2)->getString(), line));
  }

  std::shared_ptr<Expr> parseListVarExpr(std::shared_ptr<sexpresso::Sexp> sexp,
                                         int line) {
    std::vector<std::shared_ptr<Expr>> vars;
    for (int i = 1; i < sexp->childCount(); i++) {
      vars.push_back(parseExpr(sexp->getChild(i), line));
    }
    return std::make_shared<ListVarExpr>(vars);
  }

  std::shared_ptr<Expr> parseUnaryOp(std::shared_ptr<sexpresso::Sexp> sexp,
                                     int line) {
    if (sexp->childCount() == 1) {
      auto toks = Lexer(sexp->getString()).scanTokens();
      int num = std::stoi(toks[1].lexeme);
      return std::make_shared<Unary>(
          Token(toks[0].type, toks[0].lexeme, toks[0].literal, line),
          std::make_shared<Literal>(num));
    }
    auto tok = Lexer(sexp->getChild(0)->getString()).scanTokens()[0];
    return std::make_shared<Unary>(
        Token(tok.type, tok.lexeme, tok.literal, line),
        parseExpr(sexp->getChild(1), line));
  }

  std::shared_ptr<Expr> parseFunction(std::shared_ptr<sexpresso::Sexp> sexp,
                                      int line) {
    std::vector<Token> params;
    auto child1 = sexp->getChild(1);
    if (!child1->isSexp() || child1->childCount() == 0) {
      Token param = Lexer(child1->getString()).scanTokens()[0];
      if (sexp->childCount() < 3) {
        throw std::runtime_error(
            "ParseError: Function must have a body, instead got: " +
            sexp->toString());
      }
      if (param.type == TokenType::EOF_TOKEN) { // Empty function
        return std::make_shared<Lambda>(params, parseExpr(sexp->getChild(2)));
      }
      if (param.type != TokenType::IDENTIFIER) {
        throw std::runtime_error("ParseError: Function parameter must be an "
                                 "identifier, instead, token type is " +
                                 TokenTypeToString(param.type));
      }
      params.push_back(Token(param.type, param.lexeme, param.literal, line));
    } else {
      for (int i = 0; i < child1->childCount(); i++) {
        auto tok = Lexer(child1->getChild(i)->getString()).scanTokens()[0];
        if (tok.type != TokenType::IDENTIFIER) {
          throw std::runtime_error("ParseError subchild: Function parameter "
                                   "must be an identifier, instead, "
                                   "token type is " +
                                   TokenTypeToString(tok.type));
        }
        params.push_back(tok);
      }
    }
    return std::make_shared<Lambda>(params, parseExpr(sexp->getChild(2), line));
  }

  std::shared_ptr<Expr> parseLetExpr(std::shared_ptr<sexpresso::Sexp> sexp,
                                     int line = -1) {

    /*
     * let (expr1 expr2 expr3)
     */
    /* let expr1 expr2 expr3 */
    std::vector<std::shared_ptr<Expr>> exprs;
    for (int i = 1; i < sexp->childCount(); i++) {
      exprs.push_back(parseExpr(sexp->getChild(i), line));
    }
    return std::make_shared<Let>(exprs);
  }

  std::shared_ptr<Expr> parseExpr(std::shared_ptr<sexpresso::Sexp> sexp,
                                  int line = -1) {
    auto head = sexp;
    if (!sexp->isString()) {
      head = sexp->getChild(0);
    }
    if (head->isString()) {
      Token tok = Lexer(head->getString()).scanTokens()[0];
      if (tok.type == TokenType::COLON) {
        return parseTypeDecl(sexp, line);
      } else if (tok.type == TokenType::LET) {
        return parseLetExpr(sexp, line);
      } else if (tok.type == TokenType::IF) {
        return parseIfExpr(sexp, line);
      } else if (isBinaryOp(tok.type) && sexp->childCount() == 3) {
        return parseBinaryExpr(sexp, line);
      } else if (isLogicalOp(tok.type) && sexp->childCount() == 3) {
        return parseLogicalExpr(sexp, line);
      } else if (isUnaryOp(tok.type) && sexp->childCount() <= 2) {
        return parseUnaryOp(sexp, line);
      } else if (tok.type == TokenType::INITNEXT) {
        return parseInitNextExpr(sexp, line);
      } else if (tok.type == TokenType::EQUAL) {
        return parseAssignExpr(sexp, line);
      } else if (tok.type == TokenType::DOTDOT) {
        return parseGetExpr(sexp, line);
      } else if (tok.type == TokenType::NUMBER) {
        return std::make_shared<Literal>(std::stoi(tok.lexeme));
      } else if (tok.type == TokenType::STRING) {
        return std::make_shared<Literal>(tok.lexeme);
      } else if (tok.type == TokenType::FUN) {
        return parseFunction(sexp, line);
      } else if (tok.type == TokenType::MAPTO) {
        return parseFunction(sexp, line);
      } else if (tok.type == TokenType::TRUE) {
        return std::make_shared<Literal>(true);
      } else if (tok.type == TokenType::FALSE) {
        return std::make_shared<Literal>(false);
      } else if (tok.type == TokenType::IDENTIFIER) {
        if (tok.lexeme == "list") {
          return parseListVarExpr(sexp, line);
        } else {
          if (sexp->childCount() == 1) {
            return std::make_shared<Variable>(tok);
          }
          std::vector<std::shared_ptr<Expr>> args;
          for (int i = 1; i < sexp->childCount(); i++) {
            args.push_back(parseExpr(sexp->getChild(i), line));
          }
          return std::make_shared<Call>(std::make_shared<Variable>(tok), args);
        }
      } else {
        throw std::runtime_error("ParseError: Invalid expression" +
                                 sexp->toString() +
                                 ", Head: " + head->toString());
      }
    } else {
      head = sexp->getChild(0);
      std::shared_ptr<Expr> expr = parseExpr(head, line);
      std::vector<std::shared_ptr<Expr>> args;
      auto getExpr = std::dynamic_pointer_cast<Get>(expr);
      auto varExpr = std::dynamic_pointer_cast<Variable>(expr);
      if (getExpr == nullptr && varExpr == nullptr) {
        throw std::runtime_error("ParseError: Invalid expression:" +
                                 head->toString());
      }
      for (int i = 1; i < sexp->childCount(); i++) {
        args.push_back(parseExpr(sexp->getChild(i), line));
      }
      return std::make_shared<Call>(expr, args);
    }
  }

  std::shared_ptr<Stmt> parseObjectStmt(std::shared_ptr<sexpresso::Sexp> sexp,
                                        int line = -1) {
    auto name = sexp->getChild(1);
    if (!name->isString()) {
      throw std::runtime_error("ParseError: Object name must be a string");
    }
    std::vector<std::shared_ptr<Expr>> fields;
    std::shared_ptr<Expr> cexpr;
    std::shared_ptr<Expr> expr = nullptr;
    for (int i = 2; i < sexp->childCount(); i++) {
      auto field_sexp = sexp->getChild(i);
      try {
        expr = parseExpr(field_sexp, line);
      } catch (std::runtime_error &e) {
        throw std::runtime_error("ParseError: Cannot parse " +
                                 field_sexp->toString() + " at line " +
                                 std::to_string(line));
      }
      std::shared_ptr<TypeDecl> texpr =
          std::dynamic_pointer_cast<TypeDecl>(expr);
      if (texpr != nullptr) {
        fields.push_back(texpr);
      } else {
        cexpr = expr;
      }
    }
    // Check if Cell is present
    if (cexpr == nullptr) {
      throw std::runtime_error("ParseError: Cell field is required");
    }
    return std::make_shared<Object>(Token(TokenType::IDENTIFIER,
                                          name->getString(), name->getString(),
                                          line),
                                    fields, cexpr);
  }

  std::shared_ptr<Stmt> parseOnStmt(std::shared_ptr<sexpresso::Sexp> sexp,
                                    int line = -1) {
    return std::make_shared<OnStmt>(parseExpr(sexp->getChild(1)),
                                    parseExpr(sexp->getChild(2)));
  }

  std::shared_ptr<Stmt> parseStmt(std::shared_ptr<sexpresso::Sexp> sexp,
                                  int line = -1) {
    auto head = sexp->getChild(0);
    auto tok = Lexer(head->getString()).scanTokens()[0];
    if (head->isString()) {
      if (tok.type == TokenType::OBJECT) {
        return parseObjectStmt(sexp, line);
      } else if (tok.type == TokenType::ON) {
        return parseOnStmt(sexp, line);
      } else {
        return std::make_shared<Expression>(parseExpr(sexp, line));
      }
    } else {
      throw std::runtime_error("ParseError: Invalid statement");
    }
  }

  std::vector<std::shared_ptr<Stmt>> parseStmt() {
    // std::cout << "Parsing source: " << source << std::endl;
    std::shared_ptr<sexpresso::Sexp> sexp;
    try {
      sexp = sexpresso::parse(source);
    } catch (std::runtime_error &e) {
      throw std::runtime_error("ParseError: Cannot parse source into sexprs");
    }
    std::vector<std::shared_ptr<Stmt>> stmts;
    auto program_sexp = sexp->getChild(0);
    if (!program_sexp->isSexp()) {
      throw std::runtime_error(
          "ParseError: Program must be a list of statements, instead got " +
          program_sexp->getString());
    }
    for (int i = 1; i < program_sexp->childCount(); i++) {
      auto stmt_sexp = program_sexp->getChild(i);
      stmts.push_back(parseStmt(stmt_sexp, i));
    }
    return stmts;
  }
};
} // namespace Autumn

#endif
