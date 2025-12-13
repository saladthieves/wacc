#pragma once

#include "args.hpp"
#include <vector>

namespace wacc {
namespace driver {
// TODO: Add missing driver tests
void runDriver(std::vector<std::string>& arguments);
// TODO: Add cleanup tests
void cleanUp(const DriverArgs& args, bool failed);
} // namespace driver
} // namespace wacc