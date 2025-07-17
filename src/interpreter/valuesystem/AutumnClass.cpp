#include "AutumnClass.hpp"
#include "AutumnInstance.hpp"

namespace Autumn {

std::shared_ptr<AutumnValue>
AutumnMethod::call(Interpreter &interpreter,
                   const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (instance == nullptr) {
    throw Error("Calling Method Error: instance is null");
  }
  // Create a new environment for the lambda's execution
  auto environment =
      std::make_shared<Environment>(interpreter.getEnvironment(), EnvironmentType::INSTANCE);

  // Bind the new environment to the lambda's closure
  interpreter.setEnvironment(environment);
  // Bind the instance to the lambda's closure
  for (auto fieldname : instance->getClass()->getFieldNames()) {
    environment->define(fieldname, instance->get(fieldname));
  }
  // Execute the lambda's body within the new environment
  std::shared_ptr<AutumnValue> retVal =
      std::any_cast<std::shared_ptr<AutumnValue>>(
          callable->call(interpreter, arguments));
  // Restore the previous environment
  interpreter.setEnvironment(environment->getEnclosing());
  return retVal;
}

std::string AutumnMethod::toString() const {
  std::string instanceString =
      (instance == nullptr ? "" : instance->getClassName());
  if (instanceString != "") {
    for (auto fieldname : instance->getClass()->getFieldNames()) {
      instanceString += fieldname + ":" + instance->get(fieldname)->toString();
    }
  }
  std::string resultString =
      std::string("<method: >") +
      (instance == nullptr ? "" : "bound to instance " + instanceString);
  return resultString;
}

} // namespace Autumn
