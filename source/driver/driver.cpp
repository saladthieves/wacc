#include "driver.hpp"
#include "args.hpp"
#include "assembler.hpp"
#include "compiler.hpp"
#include "preprocessor.hpp"

namespace wacc::driver {
void runDriver(std::vector<std::string>& args) {
    const auto arguments = parseDriverArgs(args);
    const auto preprocessed = runPreprocessor(arguments);
    const auto assembly = runCompiler(preprocessed, arguments);
    runAssembler(assembly, arguments);
}
} // namespace wacc::driver