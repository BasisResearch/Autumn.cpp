#include "AutumnCallableValue.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

namespace Autumn {
std::shared_ptr<AutumnValue>
Print::call(Interpreter &interpreter,
            const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 1) {
    if (interpreter.getVerbose()) {
      std::cerr << "Print() takes 2 arguments, instead got " << arguments.size()
                << std::endl;
    }
    throw Error("Any() takes 2 arguments");
  }
  std::shared_ptr<AutumnValue> value = arguments[0];
#ifdef AUTUMN_DEV_MODE
  std::cout << "PRINTING: " << value->toString() << std::endl;
#endif
  return value;
}

} // namespace Autumn
