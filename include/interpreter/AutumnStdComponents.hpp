#ifndef __AUTUMN_STD_COMPONENTS_HPP__
#define __AUTUMN_STD_COMPONENTS_HPP__

#include "AutumnCallable.hpp"
#include "AutumnClass.hpp"
#include "AutumnInstance.hpp"
#include "Expr.hpp"
#include <memory>

namespace Autumn {
class Interpreter;
extern std::shared_ptr<AutumnClass> PositionClass;
extern std::shared_ptr<AutumnClass> CellClass;
extern std::shared_ptr<AutumnClass> RenderedElemClass;
std::shared_ptr<AutumnType> makeObjectClass(
    Interpreter &interpreter, std::string name,
    std::vector<std::pair<std::string, std::shared_ptr<AutumnType>>> &fields,
    std::shared_ptr<Expr> cexpr);
} // namespace Autumn
#endif
