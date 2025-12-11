#include "utils.hpp"

#include <__ostream/print.h>
#include <gtest/gtest.h>

using wacc::utils::getFileInfo;

TEST(UtilsTest, getFileInfo) {
    // ARRANGE
    const auto path = "some/path/to/file.c";

    // ACT
    auto info = getFileInfo(path);

    // ASSERT
    ASSERT_STREQ(info.fileName.c_str(), "file.c");
    ASSERT_STREQ(info.stem.c_str(), "file");
    ASSERT_STREQ(info.extension.c_str(), ".c");
    ASSERT_STREQ(info.parent.c_str(), "some/path/to");
}

TEST(UtilsTest, getFileInfoNoExt) {
    // ARRANGE
    const auto path = "some/path/to/file";

    // ACT
    auto info = getFileInfo(path);

    // ASSERT
    ASSERT_STREQ(info.fileName.c_str(), "file");
    ASSERT_STREQ(info.stem.c_str(), "file");
    ASSERT_STREQ(info.extension.c_str(), "");
    ASSERT_STREQ(info.parent.c_str(), "some/path/to");
}

TEST(UtilsTest, testDecorationChar) {
    // ARRANGE
    const auto lineNo = 8;
    const std::string line = "int main(void) {";
    const auto offset = 5;
    const std::string_view value = "main";

    // ACT
    auto output = wacc::utils::decorate(lineNo, line, offset, value);

    // ASSERT
    ASSERT_TRUE(output.contains("Line [8]: int main(void) {"));
    ASSERT_TRUE(output.contains("              ^^^^\n"));
    
}

TEST(UtilsTest, testDecorationToken) {
    // ARRANGE
    const auto lineNo = 2;
    const std::string line = "int main(void) {\n  returned 15; }";
    const auto offset = 31;
    const std::string_view value = "returned";

    // ACT
    auto output = wacc::utils::decorate(lineNo, line, offset, value);

    // ASSERT
    ASSERT_TRUE(output.contains("Line [2]: int main(void) {\n  returned 15; }"));
    ASSERT_TRUE(output.contains("                              ^^^^^^^^"));
}