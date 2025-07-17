// Author: Isak Andersson 2016 bitpuffin dot com

// If you don't opt out, copy and paste dependencies before include
#ifndef SEXPRESSO_HEADER
#define SEXPRESSO_HEADER
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
// #include "sexpresso.hpp"

namespace sexpresso {
enum class SexpValueKind : uint8_t { SEXP, STRING };

struct SexpArgumentIterator;

struct Sexp {
  Sexp();
  Sexp(std::string const &strval);
  Sexp(std::vector<Sexp> const &sexpval);
  SexpValueKind kind;
  struct {
    std::vector<std::shared_ptr<Sexp>> sexp;
    std::string str;
  } value;
  auto addChild(std::shared_ptr<Sexp> sexp) -> void;
  auto addChild(std::string str) -> void;
  auto addChildUnescaped(std::string str) -> void;
  auto addExpression(std::string const &str) -> void;
  auto childCount() const -> size_t;
  auto getChild(size_t idx)
      -> std::shared_ptr<Sexp>; // Call only if Sexp is a Sexp
  auto getString() -> std::string &;
  auto getString() const -> const std::string &;
  auto createPath(std::vector<std::string> const &path) -> Sexp &;
  auto createPath(std::string const &path) -> Sexp &;
  auto toString() const -> std::string;
  auto isString() const -> bool;
  auto isSexp() const -> bool;
  auto isNil() const -> bool;
  auto arguments() -> SexpArgumentIterator;
  static auto unescaped(std::string strval) -> std::shared_ptr<Sexp>;
};

auto parse(std::string const &str, std::string &err) -> std::shared_ptr<Sexp>;
auto parse(std::string const &str) -> std::shared_ptr<Sexp>;
auto escape(std::string const &str) -> std::string;
auto printShouldNeverReachHere() -> void;

} // namespace sexpresso

#endif
