#pragma once

#include "args.hpp"

#include <string>

namespace wacc {
namespace driver {
class CompilerResult {
public:
    bool proceed{false};
    std::string path{};
};

CompilerResult runCompiler(const std::string& preprocessed,
                        const DriverArgs& args);
} // namespace driver
} // namespace wacc