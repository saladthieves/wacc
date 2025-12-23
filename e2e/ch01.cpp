#include "cmd.hpp"
#include "driver.hpp"

#include <expected>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

using std::expected;
using std::string;
using std::string_view;
using std::unexpected;
using std::vector;
using std::filesystem::path;

class Chapter1Test : public testing::Test {
protected:
    static void SetUpTestSuite() {
        TearDownTestSuite();
        std::filesystem::create_directory(root);
    }

    static void TearDownTestSuite() {
        if (std::filesystem::exists(root)) {
            std::filesystem::remove_all(root);
        }
    }

    expected<path, string> compile(string_view code) {
        auto sourceFile = root / name;
        writeToFile(code, sourceFile);

        vector<string> args{sourceFile};

        try {
            wacc::driver::runDriver(args);
            return root / binary;
        } catch (const std::exception& ex) {
            return unexpected<string>(ex.what());
        }
    }

    int run(path& binaryPath) {
        auto result = wacc::utils::runCommand(binaryPath.string(), {});
        return result.exitCode;
    }

private:
    void writeToFile(string_view code, path& sourceFile) {
        std::stringstream buffer{};
        buffer << code;

        std::ofstream ostream{sourceFile};
        ostream << buffer.rdbuf();

        ostream.close();
    }

    static path root;
    static constexpr auto name = "program.c";
    static constexpr auto binary = "program";
};

path Chapter1Test::root = std::filesystem::absolute("sample") / "ch01";

TEST_F(Chapter1Test, chapter1e2e) {
    // ARRANGE
    vector<string> tests = {
        // clang-format off
        "int main(void) { return 4; }", 
        "int main(void) { return 8; }", 
        "int main(void) { return 15; }",
        "int main(void) { return 16; }",
        "int main(void) { return 23; }",
        "int main(void) { return 42; }",
        "int main(void) { return 25; }",
        "int main(void) { return 88; }",
        "int main(void) { return 71; }",
        "int main(void) { return 99; }",
        "int main(void) { return 101; }",
        "int main(void) { return 19; }",
        // clang-format on
    };

    for (const auto& code : tests) {
        // ACT
        auto compiled = compile(code);
        EXPECT_TRUE(compiled.has_value());

        // ASSERT
        auto binaryPath = compiled.value();
        auto exitCode = run(binaryPath);
        ASSERT_NE(exitCode, 0);
    }
}