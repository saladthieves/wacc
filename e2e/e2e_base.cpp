#include "e2e_base.hpp"
#include "cmd.hpp"
#include "driver.hpp"
#include "utils.hpp"

#include <fstream>
#include <sstream>
#include <string>

namespace wacc::test::e2e {
path rootPath() {
    return std::filesystem::absolute(e2eSampleRoot);
}

path sourcePath(string_view chapter) {
    return rootPath() / chapter / name;
}

void setUpSuite(string_view chapter) {
    auto path = rootPath() / chapter;
    if (std::filesystem::exists(path)) {
        std::filesystem::remove_all(path);
    }
    std::filesystem::create_directories(path);
}

void tearDownSuite(string_view chapter) {
    auto path = rootPath() / chapter;
    if (!std::filesystem::exists(path)) return;
    std::filesystem::remove_all(path);
}

void writeToFile(string_view code, const path& sourceFile) {
    std::stringstream buffer{};
    buffer << code;

    std::ofstream ostream{sourceFile};
    ostream << buffer.rdbuf();

    ostream.close();
}

expected<path, string> compile(string_view code, string_view chapter) {
    auto sourceFile = rootPath() / chapter / name;
    writeToFile(code, sourceFile);

    vector<string> args{sourceFile};

    try {
        wacc::driver::runDriver(args);
        return rootPath() / chapter / binary;
    } catch (const std::exception& ex) {
        return unexpected<string>(ex.what());
    }
}

int run(const path& binaryPath) {
    std::initializer_list<string> args = {binaryPath, exitCodePath};
    auto result = wacc::utils::runCommand(testerPath, args);
    auto value = utils::readFile(exitCodePath);
    value = value.erase(value.find_last_not_of('\n') + 1);
    try {
        return std::stoi(value);
    } catch (const std::exception& ex) {
        return result.exitCode;
    }
}
} // namespace wacc::test::e2e
