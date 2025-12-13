#pragma once

#include "args.hpp"

#include <string>

namespace wacc {
namespace driver {
// TODO: Add missing successful tests
std::string runCompiler(const std::string& preprocessed,
                        const DriverArgs& args);
} // namespace driver
} // namespace wacc