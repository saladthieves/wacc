#pragma once

#include "args.hpp"

namespace wacc {
namespace driver {
// TODO: Add tests 
void runAssembler(const std::string& assembly, const DriverArgs& args);
} // namespace driver
} // namespace wacc