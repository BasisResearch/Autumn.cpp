// bindings.cpp
#include "AutumnValue.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include <emscripten/bind.h>
#include <memory>
#include <string>
#include <vector>

using namespace emscripten;

// Binding for Interpreter
class InterpreterWrapper {
public:
  std::shared_ptr<Autumn::Interpreter> interpreter;

  InterpreterWrapper() {
    interpreter = std::make_shared<Autumn::Interpreter>();
  }

  std::string runScript(const std::string &script,
                        const std::string &stdlib = "",
                        const std::string &triggeringCondition = "",
                        uint64_t randomSeed = 42) {
    Autumn::SExpParser parser(script);
    std::vector<std::shared_ptr<Autumn::Stmt>> stmts = parser.parseStmt();
    try {
      interpreter->start(stmts, stdlib, triggeringCondition, randomSeed);
      return std::string("Success");
    } catch (const std::runtime_error &e) {
      return std::string("Runtime Error: ") + e.what();
    }
  }

  void step() {
    try {
      interpreter->step();
    } catch (const std::runtime_error &e) {
      std::cerr << "Runtime Error: " << e.what() << std::endl;
    }
  }

  bool getTriggerState() { return interpreter->getTriggerState(); }

  void click(int x, int y) { interpreter->getState()->click(x, y); }
  void left() { interpreter->getState()->pushLeft(); }
  void right() { interpreter->getState()->pushRight(); }
  void up() { interpreter->getState()->pushUp(); }
  void down() { interpreter->getState()->pushDown(); }

  std::string renderAll() { return interpreter->renderAll(); }
  void setVerbose(bool verbose) { interpreter->setVerbose(verbose); }
  bool getVerbose() { return interpreter->getVerbose(); }

  void setRandomSeed(uint64_t seed) { interpreter->setRandomSeed(seed); }
  uint64_t getRandomSeed() { return interpreter->getRandomGenerator()->getSeed(); }

  std::string getBackground() { return interpreter->getBackground(); }
  int getFrameRate() { return interpreter->getFrameRate(); }

  std::string evaluateToString(std::string expr) { return interpreter->evaluateToString(expr); }
  int getOnClauseCount() { return interpreter->getOnClauseCount(); }
  int getCoveredOnClauseCount() { return interpreter->getCoveredOnClauseCount(); }
};

EMSCRIPTEN_BINDINGS(my_module) {
  class_<InterpreterWrapper>("Interpreter")
      .constructor()
      .function("runScript", &InterpreterWrapper::runScript)
      .function("step", &InterpreterWrapper::step)
      .function("click", &InterpreterWrapper::click)
      .function("left", &InterpreterWrapper::left)
      .function("right", &InterpreterWrapper::right)
      .function("up", &InterpreterWrapper::up)
      .function("down", &InterpreterWrapper::down)
      .function("getTriggerState", &InterpreterWrapper::getTriggerState)
      .function("renderAll", &InterpreterWrapper::renderAll)
      .function("setVerbose", &InterpreterWrapper::setVerbose)
      .function("getVerbose", &InterpreterWrapper::getVerbose)
      .function("setRandomSeed", &InterpreterWrapper::setRandomSeed)
      .function("getRandomSeed", &InterpreterWrapper::getRandomSeed)
      .function("getBackground", &InterpreterWrapper::getBackground)
      .function("getFrameRate", &InterpreterWrapper::getFrameRate)
      .function("evaluateToString", &InterpreterWrapper::evaluateToString)
      .function("getOnClauseCount", &InterpreterWrapper::getOnClauseCount)
      .function("getCoveredOnClauseCount", &InterpreterWrapper::getCoveredOnClauseCount);
}
