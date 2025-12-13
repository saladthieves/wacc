#pragma once

#include "args.hpp"
#include <vector>

namespace wacc {
namespace driver {
void runDriver(std::vector<std::string>& arguments);
void cleanUp(const DriverArgs& args, bool failed);
} // namespace driver
} // namespace wacc