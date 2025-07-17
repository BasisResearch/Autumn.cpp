#include "AutumnCallableValue.hpp"
#include "Interpreter.hpp"
#include <vector>

namespace Autumn {
    class FunctionCollector: public Expr::Visitor, public Stmt::Visitor, public std::enable_shared_from_this<FunctionCollector> {
    public:
        std::shared_ptr<Interpreter> interpreter;

        FunctionCollector(std::shared_ptr<Interpreter> interpreter) : interpreter(interpreter) {} // Assuming the interpreter is already initialized and ran through the script

        std::vector<std::shared_ptr<Stmt>> collectFunctions(std::shared_ptr<Stmt> stmt) {
            std::vector<std::shared_ptr<Stmt>> functions;
            stmt->accept(*this);
            return functions;
        }

        std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override {
            return std::make_shared<std::vector<std::shared_ptr<AutumnCallableValue>>>();
        }

        std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override {
            throw Error("Grouping expression is not supposed to be used in this language");
        }

        std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override {
            auto res_right = expr->right->accept(*this);
            return res_right;
        }

        std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override {
            auto res_left = expr->left->accept(*this);
            auto res_right = expr->right->accept(*this);
            std::shared_ptr<std::vector<std::shared_ptr<AutumnCallableValue>>> res_left_vector = std::any_cast<std::shared_ptr<std::vector<std::shared_ptr<AutumnCallableValue>>>>(res_left);
            std::shared_ptr<std::vector<std::shared_ptr<AutumnCallableValue>>> res_right_vector = std::any_cast<std::shared_ptr<std::vector<std::shared_ptr<AutumnCallableValue>>>>(res_right);
            // Combine the two vectors
            std::vector<std::shared_ptr<AutumnCallableValue>> combined_vector;
            combined_vector.reserve(res_left_vector->size() + res_right_vector->size());
            combined_vector.insert(combined_vector.end(), res_left_vector->begin(), res_left_vector->end());
            combined_vector.insert(combined_vector.end(), res_right_vector->begin(), res_right_vector->end());
            return std::make_shared<std::vector<std::shared_ptr<AutumnCallableValue>>>(combined_vector);
        }

        std::any visitVariableExpr(std::shared_ptr<Variable> expr) override {
            // Check if this variable points to a function
            if (interpreter->getEnvironment()->get(expr->name) != nullptr) {
                return std::any_cast<std::shared_ptr<std::vector<std::shared_ptr<AutumnCallableValue>>>>(interpreter->getEnvironment()->get(expr->name));
            }
            return std::make_shared<std::vector<std::shared_ptr<AutumnCallableValue>>>();
        }

        std::any visitCallExpr(std::shared_ptr<Call> expr) override {
            // Get the function
            auto res_function = expr->callee->accept(*this);
            // If it's a variable, then it would return 
            
            std::shared_ptr<std::vector<std::shared_ptr<AutumnCallableValue>>> res_function_vector = std::any_cast<std::shared_ptr<std::vector<std::shared_ptr<AutumnCallableValue>>>>(res_function);
            // Find the function name

            // Crucial: How to recursively descent into the call function and arguments, and find the subfunctions being called?
            // Depending on the type of function, we descent differently
            // Primitive function: we can have a default map
            // Lambda function: we descent into each argument and expr
            // Object function: we descent into each argument and expr

            // Get the arguments
            std::vector<std::shared_ptr<AutumnCallableValue>> arguments;
            for (auto &argument : expr->arguments) {
                auto res_argument = argument->accept(*this);
                std::shared_ptr<std::vector<std::shared_ptr<AutumnCallableValue>>> res_argument_vector = std::any_cast<std::shared_ptr<std::vector<std::shared_ptr<AutumnCallableValue>>>>(res_argument);
                arguments.insert(arguments.end(), res_argument_vector->begin(), res_argument_vector->end());
            }
            // Call the function
            for (auto &callable : *res_function_vector) {
                callable->call(arguments);
            }
            return std::make_shared<std::vector<std::shared_ptr<AutumnCallableValue>>>();
        }

    };
}