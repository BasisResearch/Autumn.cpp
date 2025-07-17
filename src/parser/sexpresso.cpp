#include "sexpresso.hpp"
#include <cstdint>
#include <string>
#include <vector>

#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <sstream>
#include <stack>

namespace sexpresso {
Sexp::Sexp() { this->kind = SexpValueKind::SEXP; }
Sexp::Sexp(std::string const &strval) {
  this->kind = SexpValueKind::STRING;
  this->value.str = escape(strval);
}
Sexp::Sexp(std::vector<Sexp> const &sexpval) {
  this->kind = SexpValueKind::SEXP;
  this->value.sexp = std::vector<std::shared_ptr<Sexp>>{};
  for (auto &&s : sexpval)
    this->value.sexp.push_back(std::make_shared<Sexp>(s));
}

auto Sexp::addChild(std::shared_ptr<Sexp> sexp) -> void {
  if (this->kind == SexpValueKind::STRING) {
    this->kind = SexpValueKind::SEXP;
    this->value.sexp.push_back(std::make_shared<Sexp>(this->value.str));
  }
  this->value.sexp.push_back(sexp);
}

auto Sexp::addChild(std::string str) -> void {
  this->addChild(std::make_shared<Sexp>(str));
}

auto Sexp::addChildUnescaped(std::string str) -> void {
  this->addChild(Sexp::unescaped(str));
}

auto Sexp::addExpression(std::string const &str) -> void {
  auto err = std::string{};
  auto sexp = parse(str, err);
  if (!err.empty())
    return;
  for (auto &&c : sexp->value.sexp)
    this->addChild(c);
}

auto Sexp::childCount() const -> size_t {
  switch (this->kind) {
  case SexpValueKind::SEXP:
    return this->value.sexp.size();
  case SexpValueKind::STRING:
    return 1;
  }
  printShouldNeverReachHere();
  return 0;
}

static auto splitPathString(std::string const &path)
    -> std::vector<std::string> {
  auto paths = std::vector<std::string>{};
  if (path.size() == 0)
    return paths;
  auto start = path.begin();
  for (auto i = path.begin() + 1; i != path.end(); ++i) {
    if (*i == '/') {
      paths.push_back(std::string{start, i});
      start = i + 1;
    }
  }
  paths.push_back(std::string{start, path.end()});
  return paths;
}

auto Sexp::getChild(size_t idx) -> std::shared_ptr<Sexp> {
  if (idx >= this->value.sexp.size()) {
    throw std::out_of_range("Sexp::getChild: Index out of bounds, childSize " +
                            std::to_string(this->value.sexp.size()) +
                            " index " + std::to_string(idx) +
                            "\nSexp: " + this->toString());
  }
  return this->value.sexp[idx];
}

auto Sexp::getString() -> std::string & { return this->value.str; }

auto Sexp::getString() const -> const std::string & { return this->value.str; }

static const std::array<char, 11> escape_chars = {
    '\'', '"', '?', '\\', 'a', 'b', 'f', 'n', 'r', 't', 'v'};
static const std::array<char, 11> escape_vals = {
    '\'', '"', '\?', '\\', '\a', '\b', '\f', '\n', '\r', '\t', '\v'};

static auto isEscapeValue(char c) -> bool {
  return std::find(escape_vals.begin(), escape_vals.end(), c) !=
         escape_vals.end();
}

static auto countEscapeValues(std::string const &str) -> size_t {
  return std::count_if(str.begin(), str.end(), isEscapeValue);
}

static auto stringValToString(std::string const &s) -> std::string {
  if (s.size() == 0)
    return std::string{"\"\""};
  if ((std::find(s.begin(), s.end(), ' ') == s.end()) &&
      countEscapeValues(s) == 0)
    return s;
  return ('"' + escape(s) + '"');
}

static auto toStringImpl(std::shared_ptr<Sexp> sexp,
                         std::ostringstream &ostream) -> void {
  switch (sexp->kind) {
  case SexpValueKind::STRING:
    ostream << stringValToString(sexp->value.str);
    break;
  case SexpValueKind::SEXP:
    switch (sexp->value.sexp.size()) {
    case 0:
      ostream << "()";
      break;
    case 1:
      ostream << '(';
      toStringImpl(sexp->value.sexp[0], ostream);
      ostream << ')';
      break;
    default:
      ostream << '(';
      for (auto i = sexp->value.sexp.begin(); i != sexp->value.sexp.end();
           ++i) {
        toStringImpl(*i, ostream);
        if (i != sexp->value.sexp.end() - 1)
          ostream << ' ';
      }
      ostream << ')';
    }
  }
}

auto Sexp::toString() const -> std::string {
  auto ostream = std::ostringstream{};
  // outer sexp does not get surrounded by ()
  switch (this->kind) {
  case SexpValueKind::STRING:
    ostream << stringValToString(this->value.str);
    break;
  case SexpValueKind::SEXP:
    for (auto i = this->value.sexp.begin(); i != this->value.sexp.end(); ++i) {
      toStringImpl(*i, ostream);
      if (i != this->value.sexp.end() - 1)
        ostream << ' ';
    }
  }
  return ostream.str();
}

auto Sexp::isString() const -> bool {
  return this->kind == SexpValueKind::STRING;
}

auto Sexp::isSexp() const -> bool { return this->kind == SexpValueKind::SEXP; }

auto Sexp::isNil() const -> bool {
  return this->kind == SexpValueKind::SEXP && this->childCount() == 0;
}

auto Sexp::unescaped(std::string strval) -> std::shared_ptr<Sexp> {
  auto s = std::make_shared<Sexp>();
  s->kind = SexpValueKind::STRING;
  s->value.str = strval;
  return s;
}

auto parse(std::string const &str, std::string &err) -> std::shared_ptr<Sexp> {
  auto sexprstack = std::stack<std::shared_ptr<Sexp>>{};
  sexprstack.push(std::make_shared<Sexp>());
  auto nextiter = str.begin();
  for (auto iter = nextiter; iter != str.end(); iter = nextiter) {
    nextiter = iter + 1;
    if (std::isspace(*iter))
      continue;
    switch (*iter) {
    case '(':
      sexprstack.push(std::make_shared<Sexp>());
      break;
    case ')': {
      auto topsexp = sexprstack.top();
      sexprstack.pop();
      if (sexprstack.size() == 0) {
        err = std::string{"too many ')' characters detected, closing sexprs "
                          "that don't exist, no good."};
        return std::shared_ptr<Sexp>{};
      }
      auto top = sexprstack.top();
      top->addChild(topsexp);
      break;
    }
    case '"': {
      auto i = iter + 1;
      auto start = i;
      for (; i != str.end(); ++i) {
        if (*i == '\\') {
          ++i;
          continue;
        }
        if (*i == '"')
          break;
        if (*i == '\n') {
          err = std::string{"Unexpected newline in string literal"};
          return std::shared_ptr<Sexp>{};
        }
      }
      if (i == str.end()) {
        err = std::string{"Unterminated string literal"};
        return std::shared_ptr<Sexp>{};
      }
      auto resultstr = std::string{};
      resultstr.reserve(i + 2 - start);
      resultstr.push_back('"');
      for (auto it = start; it != i; ++it) {
        switch (*it) {
        case '\\': {
          ++it;
          if (it == i) {
            err = std::string{
                "Unfinished escape sequence at the end of the string"};
            return std::shared_ptr<Sexp>{};
          }
          auto pos = std::find(escape_chars.begin(), escape_chars.end(), *it);
          if (pos == escape_chars.end()) {
            err = std::string{"invalid escape char '"} + *it + '\'';
            return std::shared_ptr<Sexp>{};
          }
          resultstr.push_back(escape_vals[pos - escape_chars.begin()]);
          break;
        }
        default:
          resultstr.push_back(*it);
        }
      }
      resultstr.push_back('"');
      sexprstack.top()->addChildUnescaped(resultstr);
      nextiter = i + 1;
      break;
    }
    case ';':
      for (; nextiter != str.end() && *nextiter != '\n' && *nextiter != '\r';
           ++nextiter) {
      }
      for (; nextiter != str.end() && (*nextiter == '\n' || *nextiter == '\r');
           ++nextiter) {
      }
      break;
    default:
      auto symend = std::find_if(iter, str.end(), [](char const &c) {
        return std::isspace(c) || c == ')' || c == '(';
      });
      auto &top = sexprstack.top();
      top->addChild(std::make_shared<Sexp>(std::string{iter, symend}));
      nextiter = symend;
    }
  }
  if (sexprstack.size() != 1) {
    err = std::string{
        "not enough s-expressions were closed by the end of parsing"};
    return std::shared_ptr<Sexp>{};
  }
  return sexprstack.top();
}

auto parse(std::string const &str) -> std::shared_ptr<Sexp> {
  auto ignored_error = std::string{};
  return parse(str, ignored_error);
}

auto escape(std::string const &str) -> std::string {
  auto escape_count = countEscapeValues(str);
  if (escape_count == 0)
    return str;
  auto result_str = std::string{};
  result_str.reserve(str.size() + escape_count);
  for (auto c : str) {
    auto loc = std::find(escape_vals.begin(), escape_vals.end(), c);
    if (loc == escape_vals.end())
      result_str.push_back(c);
    else {
      result_str.push_back('\\');
      result_str.push_back(escape_chars[loc - escape_vals.begin()]);
    }
  }
  return result_str;
}

auto printShouldNeverReachHere() -> void {
  std::cerr << "Error: Should never reach here " << __FILE__ << ": " << __LINE__
            << std::endl;
}

} // namespace sexpresso
