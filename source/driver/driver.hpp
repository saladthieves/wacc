#pragma once

#include "args.hpp"
#include "utils.hpp"
#include <vector>

namespace wacc {
namespace driver {
void runDriver(std::vector<std::string>& arguments);
void cleanUp(const DriverArgs& args, const utils::FileInfo& info, bool failed);
} // namespace driver
} // namespace wacc