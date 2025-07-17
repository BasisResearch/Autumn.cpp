#include "AutumnCallableValue.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>
#include <execution>
#include <future>
#include <thread>
#include <mutex>

namespace Autumn {
std::shared_ptr<AutumnValue>
Map::call(Interpreter &interpreter,
          const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
    if (arguments.size() != 2) {
        throw Error("Map() takes 2 arguments");
    }

    // Validate callable
    auto callable = std::dynamic_pointer_cast<AutumnCallableValue>(arguments[0]);
    if (!callable) {
        throw Error("Map() first argument must be a callable");
    }

    // Validate list
    auto list = std::dynamic_pointer_cast<AutumnList>(arguments[1]);
    if (!list) {
        throw Error("Map() second argument must be a list, instead got " + 
                   arguments[1]->toString());
    }

    const auto& input_values = *list->getValues();
    const size_t size = input_values.size();

    // Choose parallelization strategy based on size
    if (size < 10000) { // 100 x 100. Disabling parallelization for now.
        return mapSequential(interpreter, callable, input_values);
    } else {
        return mapParallelSTL(interpreter, callable, input_values);
    }  // Disabling task-based parallelization for now due to unstability and inability to copy interpreter state
}

std::shared_ptr<AutumnValue> 
Map::mapSequential(Interpreter &interpreter,
                  std::shared_ptr<AutumnCallableValue> callable,
                  const std::vector<std::shared_ptr<AutumnValue>> &values) {
    auto newList = std::make_shared<AutumnList>();
    newList->getValues()->reserve(values.size());
    
    for (const auto& value : values) {
        std::vector<std::shared_ptr<AutumnValue>> args = {value};
        newList->getValues()->push_back(callable->call(interpreter, args));
    }
    
    return newList;
}

std::shared_ptr<AutumnValue> 
Map::mapParallelSTL(Interpreter &interpreter,
                   std::shared_ptr<AutumnCallableValue> callable,
                   const std::vector<std::shared_ptr<AutumnValue>> &values) {
    // For very small lists, just use sequential processing
    if (values.size() < 32) {
        return mapSequential(interpreter, callable, values);
    }
    
    auto newList = std::make_shared<AutumnList>();
    newList->getValues()->reserve(values.size());
    
    // Get a pointer to the interpreter
    Interpreter* interpreter_ptr = &interpreter;
    
    // Make a copy of the callable
    auto callable_copy = callable;
    
    // Determine hardware concurrency, with sensible defaults
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;  // Default if detection fails
    
    // Calculate chunk size - ensure reasonable chunk sizes for efficiency
    size_t chunk_size = std::max(size_t(32), values.size() / num_threads);
    
    // Store futures for async operations
    std::vector<std::future<std::vector<std::shared_ptr<AutumnValue>>>> futures;
    futures.reserve(values.size() / chunk_size + 1);
    
    // Process data in chunks
    for (size_t i = 0; i < values.size(); i += chunk_size) {
        size_t end = std::min(i + chunk_size, values.size());
        
        // Create a copy of the values in this chunk to avoid reference issues
        std::vector<std::shared_ptr<AutumnValue>> chunk_values;
        chunk_values.reserve(end - i);
        for (size_t j = i; j < end; ++j) {
            chunk_values.push_back(values[j]);
        }
        
        // Launch a task for each chunk with proper captures
        futures.push_back(std::async(
            std::launch::async,
            [interpreter_ptr, callable_copy, chunk_values = std::move(chunk_values)]() {
                std::vector<std::shared_ptr<AutumnValue>> results;
                results.reserve(chunk_values.size());
                
                // Process each item in this chunk
                for (const auto& value : chunk_values) {
                    std::vector<std::shared_ptr<AutumnValue>> args = {value};
                    results.push_back(callable_copy->call(*interpreter_ptr, args));
                }
                
                return results;
            }
        ));
    }
    
    // Collect results
    for (auto& future : futures) {
        auto chunk_results = future.get();
        newList->getValues()->insert(
            newList->getValues()->end(),
            chunk_results.begin(),
            chunk_results.end()
        );
    }
    
    return newList;
}

int Map::arity() { return 2; }
} // namespace Autumn