#include "driver.hpp"
#include "args.hpp"
#include "compiler.hpp"
#include "preprocessor.hpp"

namespace wacc::driver {
void runDriver(std::vector<std::string>& args) {
    const auto arguments = parseDriverArgs(args);
    const auto preprocessed = runPreprocessor(arguments);
    runCompiler(preprocessed, arguments);
}
} // namespace wacc::driver