

#include "AutumnCallableValue.hpp"
#include "AutumnClass.hpp"
#include "AutumnStdComponents.hpp"
#include "AutumnStdLib.hpp"
#include "AutumnValue.hpp"
#include "Environment.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include <Error.hpp>
#include <cassert>
#include <memory>

namespace Autumn {
std::shared_ptr<AutumnValue>
AdjPositions::call(Interpreter &interpreter,
                   const std::vector<std::shared_ptr<AutumnValue>> &arguments) {
  if (arguments.size() != 1) {
    throw Error("AdjPositions() takes 1 argument");
  }
  std::shared_ptr<AutumnInstance> pos = nullptr;
  if (arguments[0]->getType() != PositionClass) {
    throw Error("AdjPositions() argument must be a Position instance");
  } else {
    pos = std::dynamic_pointer_cast<AutumnInstance>(arguments[0]);
  }

  int x = std::dynamic_pointer_cast<AutumnNumber>(pos->get("x"))->getNumber();
  int y = std::dynamic_pointer_cast<AutumnNumber>(pos->get("y"))->getNumber();
  std::vector<std::pair<int, int>> adjPos = {
      {x - 1, y}, {x + 1, y}, {x, y - 1}, {x, y + 1}};
  std::vector<std::shared_ptr<AutumnValue>> adjPosValues;
  int grid_size = std::dynamic_pointer_cast<AutumnNumber>(interpreter.getEnvironment()->get("GRID_SIZE"))->getNumber();
  for (size_t i = 0; i < adjPos.size(); i++) {
    if (adjPos[i].first < 0 || adjPos[i].first >= grid_size ||
        adjPos[i].second < 0 || adjPos[i].second >= grid_size) {
      continue;
    }
    std::vector<std::shared_ptr<AutumnValue>> xy;
    xy.push_back(std::make_shared<AutumnNumber>(adjPos[i].first));
    xy.push_back(std::make_shared<AutumnNumber>(adjPos[i].second));
    adjPosValues.push_back(std::make_shared<AutumnInstance>(PositionClass, xy));
  }
  return std::make_shared<AutumnList>(std::make_shared<std::vector<std::shared_ptr<AutumnValue>>>(adjPosValues));
}

int AdjPositions::arity() { return 1; }
} // namespace Autumn
