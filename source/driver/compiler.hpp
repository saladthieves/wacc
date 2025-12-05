#pragma once

#include "args.hpp"

#include <string>

namespace wacc {
namespace driver {
void runCompiler(const std::string& preprocessed, const DriverArgs& args);
}
} // namespace wacc