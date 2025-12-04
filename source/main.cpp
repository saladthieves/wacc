#include "driver.hpp"
#include "log.hpp"

#include <string>
#include <vector>

int main(int argc, const char* argv[]) {
    using namespace wacc;
    try {
        std::vector<std::string> args;
        if (argc > 1) {
            args = {argv + 1, argv + argc};
        }
        driver::runDriver(args);
    } catch (const std::exception& exception) {
        log::e("{}", exception.what());
    }

    return 0;
}