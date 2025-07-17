#include "AutumnValue.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include <memory>
#include "jlcxx/jlcxx.hpp"
#include "jlcxx/stl.hpp"

// InterpreterWrapper class (same as Python version)
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
    } catch (const std::runtime_error &e) {
      return std::string("Runtime Error: ") + e.what();
    }
    return "Script executed successfully.";
  }

  std::string tmpExecuteStmt(const std::string &script) {
    std::shared_ptr<sexpresso::Sexp> sexp = sexpresso::parse(script);
    Autumn::SExpParser parser(script);
    std::shared_ptr<Autumn::Stmt> stmt = parser.parseStmt(sexp, -1);
    try {
      interpreter->tmpExecuteStmt(stmt);
      return interpreter->getEnvironmentString();
    } catch (const std::runtime_error &e) {
      return std::string("Runtime Error: ") + e.what();
    }
  }

  std::string getEnvironmentString() {
    return interpreter->getEnvironmentString();
  }

  void restoreEnvironment(const std::string &script, const std::string &stdlib, const std::string &triggeringCondition) {
    Autumn::SExpParser parser(script);
    std::vector<std::shared_ptr<Autumn::Stmt>> stmts = parser.parseStmt();
    try {
      interpreter->start(stmts, stdlib, triggeringCondition);
      interpreter->restoreEnvironment();
    } catch (const std::runtime_error &e) {
      throw std::runtime_error("Runtime Error: " + std::string(e.what()));
    }
  }

  std::string step() {
    interpreter->step();
    return "Step executed.";
  }

  void click(int x, int y) { interpreter->getState()->click(x, y); }
  void left() { interpreter->getState()->pushLeft(); }
  void right() { interpreter->getState()->pushRight(); }
  void up() { interpreter->getState()->pushUp(); }
  void down() { interpreter->getState()->pushDown(); }

  bool getTriggerState() { return interpreter->getTriggerState(); }

  std::string renderAll() { return interpreter->renderAll(); }
  void setVerbose(bool verbose) { interpreter->setVerbose(verbose); }
  bool getVerbose() { return interpreter->getVerbose(); }

  std::string getBackground() { return interpreter->getBackground(); }
  int getFrameRate() { return interpreter->getFrameRate(); }

  std::string evaluateToString(std::string expr) { return interpreter->evaluateToString(expr); }
  int getOnClauseCount() { return interpreter->getOnClauseCount(); }
  int getCoveredOnClauseCount() { return interpreter->getCoveredOnClauseCount(); }
};

JLCXX_MODULE define_julia_module(jlcxx::Module& mod)
{
    mod.add_type<InterpreterWrapper>("Interpreter")
        .constructor()
        .method("run_script", &InterpreterWrapper::runScript)
        .method("step", &InterpreterWrapper::step)
        .method("click", &InterpreterWrapper::click)
        .method("left", &InterpreterWrapper::left)
        .method("right", &InterpreterWrapper::right)
        .method("up", &InterpreterWrapper::up)
        .method("down", &InterpreterWrapper::down)
        .method("get_trigger_state", &InterpreterWrapper::getTriggerState)
        .method("render_all", &InterpreterWrapper::renderAll)
        .method("get_environment_string", &InterpreterWrapper::getEnvironmentString)
        .method("restore_environment", &InterpreterWrapper::restoreEnvironment)
        .method("tmp_execute_stmt", &InterpreterWrapper::tmpExecuteStmt)
        .method("set_verbose", &InterpreterWrapper::setVerbose)
        .method("get_verbose", &InterpreterWrapper::getVerbose)
        .method("get_background", &InterpreterWrapper::getBackground)
        .method("get_framerate", &InterpreterWrapper::getFrameRate)
        .method("evaluate_to_string", &InterpreterWrapper::evaluateToString)
        .method("get_on_clause_count", &InterpreterWrapper::getOnClauseCount)
        .method("get_covered_on_clause_count", &InterpreterWrapper::getCoveredOnClauseCount);
} 