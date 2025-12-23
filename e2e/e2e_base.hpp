#include <expected>
#include <filesystem>
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <vector>

using std::expected;
using std::string;
using std::string_view;
using std::unexpected;
using std::vector;
using std::filesystem::path;

namespace wacc {
namespace test {
namespace e2e {
namespace {
static constexpr auto name = "program.c";
static constexpr auto binary = "program";
static constexpr auto e2eSampleRoot = "build/e2e_sample";
} // namespace

path rootPath();

path sourcePath(string_view chapter);

void setUpSuite(string_view chapter);

void tearDownSuite(string_view chapter);

void writeToFile(string_view code, const path& sourceFile);

expected<path, string> compile(string_view cod, string_view chapter);

int run(const path& binaryPath);

} // namespace e2e
} // namespace test
} // namespace wacc