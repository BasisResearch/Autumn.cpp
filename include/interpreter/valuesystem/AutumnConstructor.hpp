#ifndef AUTUMN_CONSTRUCTOR_HPP
#define AUTUMN_CONSTRUCTOR_HPP

#include "AstPrinter.hpp"
#include "AutumnCallable.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Error.hpp"
#include "Expr.hpp"
#include "Interpreter.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Autumn {

class AutumnConstructor : public AutumnCallable {
public:
  AutumnConstructor(std::shared_ptr<Lambda> declaration,
               std::shared_ptr<Environment> closure)
      : declaration_(std::make_shared<Lambda>(*declaration)),
        closure_(closure) {
    AstPrinter printer;
    lambdaName = std::any_cast<std::string>(
        declaration_->accept(printer)); // Get the lambda's name
  }

  // Returns the number of parameters the lambda expects
  int arity() override { return declaration_->params.size(); }

  // Executes the lambda with the given arguments
  std::shared_ptr<AutumnValue>
  call(Interpreter &interpreter,
       const std::vector<std::shared_ptr<AutumnValue>> &arguments) override {
    // std::cerr << "Lambda call: " << lambdaName << std::endl;
    // Create a new environment for the lambda's execution
    auto environment =
        std::make_shared<Environment>(interpreter.getEnvironment(), EnvironmentType::INSTANCE);

    interpreter.setEnvironment(environment);
    // Bind each argument to its corresponding parameter
    for (size_t i = 0; i < declaration_->params.size(); ++i) {
      try {
        environment->define(declaration_->params[i].lexeme, arguments[i]);
      } catch (const Error &e) {
        throw Error("Error visiting: " + AstPrinter().print(declaration_) +
                    "\nGot: " + e.what());
      }
    }

    // Bind the new environment to the lambda's closure
    // Execute the lambda's body within the new environment
    try {
      std::shared_ptr<AutumnValue> retVal =
          std::any_cast<std::shared_ptr<AutumnValue>>(
              declaration_->right->accept(interpreter));

      interpreter.setEnvironment(environment->getEnclosing());
      return retVal;
    } catch (const Error &e) {
      std::string argumentStrings = "(";
      for (size_t i = 0; i < declaration_->params.size(); ++i) {
        argumentStrings +=
            "{" + declaration_->params[i].lexeme + ": " +
            environment->get(declaration_->params[i].lexeme)->toString() + "}";
      }
      argumentStrings += ")";
      throw Error("Error calling: " + AstPrinter().print(declaration_) +
                  // "\nwith arguments: " + argumentStrings + "\nGot: \n" +
                  ": " +
                  e.what());
    }
    // std::cerr << "Lambda Res: " << retVal->toString() << std::endl;
    // Restore the previous environment
  }

  // Returns a string representation of the lambda
  std::string toString() const override { return std::string(lambdaName); }

  // Returns a clone of the lambda
  std::shared_ptr<AutumnCallable> clone() override {
    return std::make_shared<AutumnConstructor>(
        std::make_shared<Lambda>(*declaration_), closure_);
  }

private:
  std::string lambdaName;
  std::shared_ptr<Lambda> declaration_;
  std::shared_ptr<Environment> closure_;
  bool isInitializer_ = false; // Determines if the lambda is an initializer
};

} // namespace Autumn

#endif