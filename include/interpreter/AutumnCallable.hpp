#ifndef _AUTUMN_CALLABLE_HPP_
#define _AUTUMN_CALLABLE_HPP_
#include "AutumnValue.hpp"
#include "Interpreter.hpp"
#include <memory>
#include <vector>
namespace Autumn {
class Interpreter;
class AutumnCallable {
public:
  virtual std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) = 0;
  virtual int arity() = 0;
  virtual ~AutumnCallable() = default;
  virtual std::string toString() const { return std::string("callable"); };
  virtual std::shared_ptr<AutumnCallable> clone() = 0;
};
} // namespace Autumn

#endif // !_AUTUMN_CALLABLE_HPP_
