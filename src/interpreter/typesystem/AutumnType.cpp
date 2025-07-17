#include "AutumnType.hpp"

namespace Autumn {
std::shared_ptr<AutumnNumberType> AutumnNumberType::instance = nullptr;
std::shared_ptr<AutumnStringType> AutumnStringType::instance = nullptr;
std::shared_ptr<AutumnBoolType> AutumnBoolType::instance = nullptr;
std::shared_ptr<AutumnUnknownType> AutumnUnknownType::instance = nullptr;
} // namespace Autumn
